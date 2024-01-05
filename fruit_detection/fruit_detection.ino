#include <TensorFlowLite.h>

#include "modules/camera.h"
#include "modules/model.h"

#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/schema/schema_generated.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/micro/micro_log.h"

// Basic TFLM entities
const tflite::Model* model = nullptr;
tflite::MicroInterpreter* interpreter = nullptr;
TfLiteTensor* inputTensor = nullptr;
TfLiteTensor* outputTensor = nullptr;

// An area of memory to use for input, output, and intermediate arrays.
alignas(16) constexpr int kTensorArenaSize = 8500 * 16;
uint8_t tensorArena[kTensorArenaSize];

void setup() {

    // Initialise hardware and entities
    tflite::InitializeTarget();
    SetUpHardware();

    // Load model
    model = tflite::GetModel(model_tflite);
    if (model->version() != TFLITE_SCHEMA_VERSION) {
        MicroPrintf(
            "Model provided is schema version %d not equal "
            "to supported version %d.",
            model->version(), TFLITE_SCHEMA_VERSION);
        return;
    }

    // Operations in the graph
    static tflite::MicroMutableOpResolver<8> opsResolver;
    opsResolver.AddMul();
    opsResolver.AddSub();
    opsResolver.AddMean();
    opsResolver.AddConv2D();
    opsResolver.AddAveragePool2D();
    opsResolver.AddDepthwiseConv2D();
    opsResolver.AddReshape();
    opsResolver.AddFullyConnected();

    // Interpreter for the model
    interpreter = new tflite::MicroInterpreter(
        model,
        opsResolver,
        tensorArena,
        kTensorArenaSize
    );

    // Allocate memory from the allocated space for the model's tensors.
    TfLiteStatus allocateStatus = interpreter->AllocateTensors();
    if (allocateStatus != kTfLiteOk) {
        MicroPrintf("Model (Error): AllocateTensors() failed");
        return;
    }

    // Get the memory area to use for the model's input.
    inputTensor = interpreter->input(0);
    if ((inputTensor->dims->size != 4) ||
        (inputTensor->dims->data[0] != 1) ||
        (inputTensor->dims->data[1] != modelHeight) ||
        (inputTensor->dims->data[2] != modelWidth) ||
        (inputTensor->dims->data[3] != modelChannels) ||
        (inputTensor->type != kTfLiteInt8)) {
        MicroPrintf("Model (Error): Bad input tensor parameters in model");
        return;
    }
}

void loop() {

    // Waits until the button is clicked
    WaitForButton();

    TfLiteStatus captureStatus = GetImage(
        inputTensor->data.int8,
        inputTensor->params.scale,
        inputTensor->params.zero_point
    );
    if (captureStatus != kTfLiteOk) {
        MicroPrintf(" - Error: Frame capture failed!");
    }

    MicroPrintf("PIXELS");
    for (int pos = 0; pos < (modelHeight * modelWidth); pos++) {
        Serial.print( (inputTensor->data.int8[3 * pos] - inputTensor->params.zero_point) * inputTensor->params.scale, 5);
        Serial.print(",");
        Serial.print( (inputTensor->data.int8[3 * pos + 1] - inputTensor->params.zero_point) * inputTensor->params.scale, 5 );
        Serial.print(",");
        Serial.print( (inputTensor->data.int8[3 * pos + 2] - inputTensor->params.zero_point) * inputTensor->params.scale, 5);
        Serial.println();
    }

    // Run the model on the loaded input.
    MicroPrintf("2) Running model.");
    if (kTfLiteOk != interpreter->Invoke()) {
        MicroPrintf("Invoke failed.");
    }
    outputTensor = interpreter->output(0);

    // Get the output meaning
    int8_t predOne = outputTensor->data.int8[0];
    int8_t predTwo = outputTensor->data.int8[1];
    int8_t predThree = outputTensor->data.int8[2];
    int8_t predFour = outputTensor->data.int8[3];
    int8_t predFive = outputTensor->data.int8[4];
    int8_t predSix = outputTensor->data.int8[5];
    MicroPrintf(" - PRED INT: [ %d, %d, %d, %d, %d, %d]", predOne, predTwo, predThree, predFour, predFive, predSix);

    float scoreOne = (predOne - outputTensor->params.zero_point) * outputTensor->params.scale;
    float scoreTwo = (predTwo - outputTensor->params.zero_point) * outputTensor->params.scale;
    float scoreThree = (predThree - outputTensor->params.zero_point) * outputTensor->params.scale;
    float scoreFour = (predThree - outputTensor->params.zero_point) * outputTensor->params.scale;
    float scoreFive = (predFive - outputTensor->params.zero_point) * outputTensor->params.scale;
    float scoreSix = (predSix - outputTensor->params.zero_point) * outputTensor->params.scale;
    Serial.print(" - PRED FLOAT: [ ");
    Serial.print(scoreOne, 3);
    Serial.print(", ");
    Serial.print(scoreTwo, 3);
    Serial.print(", ");
    Serial.print(scoreThree, 3);
    Serial.print(", ");
    Serial.print(scoreFour, 3);
    Serial.print(", ");
    Serial.print(scoreFive, 3);
    Serial.print(", ");
    Serial.print(scoreSix, 3);
    Serial.print(" ]");
    Serial.println();

}

