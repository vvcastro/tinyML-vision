#ifndef PROJECT_IMAGE_PROVIDER_H_
#define PROJECT_IMAGE_PROVIDER_H_

#include "Arduino.h"

// Type of images we are using
// VGA = 0,  // 640x480
// CIF = 1,  // 352x240
// QVGA = 2, // 320x240
// QCIF = 3,  // 176x144
// QQVGA = 4,  // 160x120
constexpr int CAMERA_SHAPE = 4;
constexpr int camPixelBytes = 2;

// Define models input shape
constexpr int modelHeight = 112;
constexpr int modelWidth = 112;
constexpr int modelChannels = 3;

// Places a quantized 8-bit image into the given tensor
bool GetImage(uint8_t* outBuffer);

// Just to wait until interaction
void WaitForButton();

// Initialises the camera in the required configurations
bool SetUpHardware();

//----------------------
// Utility expressions
//----------------------

constexpr int GetCameraHeight() {
    switch (CAMERA_SHAPE) {
        case 0:
            return 480;
        case 1:
            return 240;
        case 2:
            return 240;
        case 3:
            return 144;
        case 4:
            return 120;
    }
}

constexpr int GetCameraWidth() {
    switch (CAMERA_SHAPE) {
        case 0:
            return 640;
        case 1:
            return 352;
        case 2:
            return 320;
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