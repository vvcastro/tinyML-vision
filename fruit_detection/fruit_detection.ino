#include <TensorFlowLite.h>
#include <Arduino.h>

#include "modules/camera.h"
#include "modules/model.h"

// #include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
// #include "tensorflow/lite/schema/schema_generated.h"
// #include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/micro/micro_log.h"

// Globals, used for compatibility with Arduino-style sketches.
namespace {

    // const tflite::Model* model = nullptr;
    // tflite::MicroInterpreter* interpreter = nullptr;
    // TfLiteTensor* input = nullptr;

    // An area of memory to use for input, output, and intermediate arrays.
    // constexpr int kTensorArenaSize = 102 * 1024;
    // alignas(16) uint8_t tensorArena[kTensorArenaSize];

    // Input tensor space
    int8_t inputBuffer[96 * 96 * 3];

}

void setup() {

    // Initialise hardware and entities
    tflite::InitializeTarget();
    SetUpHardware();

    // Load model
    // model = tflite::GetModel(modelData);
    // if (model->version() != TFLITE_SCHEMA_VERSION) {
    //     MicroPrintf(
    //         "Model provided is schema version %d not equal "
    //         "to supported version %d.",
    //         model->version(), TFLITE_SCHEMA_VERSION);
    //     return;
    // }
    // MicroPrintf("Model: Model data is loaded!");

    // // Operations needed by the graph model
    // static tflite::MicroMutableOpResolver<5> microOpResolver;
    // microOpResolver.AddAveragePool2D();
    // microOpResolver.AddConv2D();
    // microOpResolver.AddDepthwiseConv2D();
    // microOpResolver.AddReshape();
    // microOpResolver.AddSoftmax();

    // // Interpreter for the model
    // static tflite::MicroInterpreter staticInterpreter(
    //     model,
    //     microOpResolver,
    //     tensorArena,
    //     kTensorArenaSize
    // );
    // interpreter = &staticInterpreter;

    // // Allocate memory from the allocated space for the model's tensors.
    // TfLiteStatus allocateStatus = interpreter->AllocateTensors();
    // if (allocateStatus != kTfLiteOk) {
    //     MicroPrintf("Model (Error): AllocateTensors() failed");
    //     return;
    // }

    // // Get the memory area to use for the model's input.
    // input = interpreter->input(0);
    // if ((input->dims->size != 4) || (input->dims->data[0] != 1) ||
    //     (input->dims->data[1] != modelRows) ||
    //     (input->dims->data[2] != modelCols) ||
    //     (input->dims->data[3] != modelChannels) || (input->type != kTfLiteInt8)) {
    //     MicroPrintf("Model (Error): Bad input tensor parameters in model");
    //     return;
    // }

}

void loop() {

    // Waits until the button is clicked
    // WaitForButton();
    // GetImage(input->data.int8);
    GetImage(inputBuffer);

    // // Run the model on the loaded input.
    // if (kTfLiteOk != interpreter->Invoke()) {
    //     MicroPrintf("Invoke failed.");
    // }
    // TfLiteTensor* output = interpreter->output(0);

    // // // Get the output meaning
    // int8_t personScore = output->data.uint8[1];
    // int8_t notPersonScore = output->data.uint8[0];
    // MicroPrintf("Model (output)|| P: %d || NP: %d", personScore, notPersonScore);

}

