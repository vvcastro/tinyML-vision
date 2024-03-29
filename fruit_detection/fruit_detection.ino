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
alignas(16) constexpr int kTensorArenaSize = 9000 * 16;
uint8_t tensorArena[kTensorArenaSize];

// To start frame acquisition
bool commandRecv = false;

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
    static tflite::MicroMutableOpResolver<10> opsResolver;
    opsResolver.AddAdd();
    opsResolver.AddMul();
    opsResolver.AddSub();
    opsResolver.AddMean();
    opsResolver.AddConv2D();
    opsResolver.AddAveragePool2D();
    opsResolver.AddDepthwiseConv2D();
    opsResolver.AddReshape();
    opsResolver.AddFullyConnected();
    opsResolver.AddQuantize();

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
        (inputTensor->type != kTfLiteUInt8)) {
        MicroPrintf("Model (Error): Bad input tensor parameters in model");
        return;
    }
}

void loop() {
    String command;

    // Read incoming commands from serial monitor
    while (Serial.available()) {
        char c = Serial.read();
        if ((c != '\n') && (c != '\r')) {
            command.concat(c);
        }
        else if (c == '\r') {
            commandRecv = true;
            command.toLowerCase();
        }
    }

    // Command interpretation
    if (commandRecv) {
        commandRecv = false;
        if (command == "capture" or command == "c") {

            TfLiteStatus captureStatus = GetImage(
                inputTensor->data.uint8,
                inputTensor->params.scale,
                inputTensor->params.zero_point
            );
            if (captureStatus != kTfLiteOk) {
                MicroPrintf(" - Error: Frame capture failed!");
            }

            // Run the model on the loaded input.
            MicroPrintf("... Running model ...");
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
            float scoreFour = (predFour - outputTensor->params.zero_point) * outputTensor->params.scale;
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
    }
}

