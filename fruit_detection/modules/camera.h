#ifndef PROJECT_IMAGE_PROVIDER_H_
#define PROJECT_IMAGE_PROVIDER_H_

#include "tensorflow/lite/c/common.h"
#include "Arduino.h"

// Define raw image shape
constexpr size_t rawCols = 176;
constexpr size_t rawRows = 144;

// Define input image shape
constexpr int modelCols = 96;
constexpr int modelRows = 96;
constexpr int modelChannels = 1;

// Number of bytes to use depending on type
enum COLOR_NAME {
    RGB_COLOR = 2,
    GRAY_COLOR = 4
};

// Initialises the camera in the required configurations
TfLiteStatus SetUpHardware();

// Places a quantized 8-bit image into the given tensor
TfLiteStatus GetImage(int8_t* imgBuffer);

// Just to wait until interaction
void WaitForButton();

constexpr size_t GetPixelBytes(const COLOR_NAME scheme) {
    switch (scheme) {
        case RGB_COLOR:
            return 2;
        case GRAY_COLOR:
            return 1;
    }
    return 0;
}

#endif