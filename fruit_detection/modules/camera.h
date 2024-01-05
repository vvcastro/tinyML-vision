#ifndef PROJECT_IMAGE_PROVIDER_H_
#define PROJECT_IMAGE_PROVIDER_H_

#include "tensorflow/lite/c/common.h"
#include "Arduino.h"

// Type of images we are using
constexpr int CAMERA_SHAPE = 3; // (QCIF = 3); (QQVGA = 4)
constexpr int camPixelBytes = 2;

// Define models input shape
constexpr int modelHeight = 112;
constexpr int modelWidth = 112;
constexpr int modelChannels = 3;

// Places a quantized 8-bit image into the given tensor
TfLiteStatus GetImage(int8_t* outBuffer, float quantScale, int32_t zeroPoint);

// Just to wait until interaction
void WaitForButton();

// Initialises the camera in the required configurations
TfLiteStatus SetUpHardware();

//----------------------
// Utility expressions
//----------------------

constexpr int GetCameraHeight() {
    switch (CAMERA_SHAPE) {
        case 3:
            return 144;
        case 4:
            return 120;
    }
}

constexpr int GetCameraWidth() {
    switch (CAMERA_SHAPE) {
        case 3:
            return 176;
        case 4:
            return 160;
    }
}

// Returns the total number of bytes in one frame of the camera
constexpr int CameraFrameSize() {
    return camPixelBytes * GetCameraWidth() * GetCameraHeight();
}

#endif