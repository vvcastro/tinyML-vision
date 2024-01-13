#include "TinyMLShield.h"
#include "modules/camera.h"

#ifndef ARDUINO_EXCLUDE_CODE

#include "Arduino.h"

uint8_t cameraBuffer[CameraFrameSize()];

// Performs several computations over the image:
// (0) Captures the image.
// (1) Crops the image to the desired model's input size.
// (2) Transform from RGB565 to RGB888, adding one byte.
// (3) Quantize the values to a int8_t type.
bool GetImage(uint8_t* outBuffer) {

    // Capture a frame and store it into the buffer
    Camera.readFrame(cameraBuffer);

    // Define the vertix for the crop
    const int rowTop = (GetCameraHeight() - modelHeight) / 2;
    const int rowBottom = rowTop + modelHeight - 1;
    const int colLeft = (GetCameraWidth() - modelWidth) / 2;
    const int colRight = colLeft + modelWidth - 1;

    // Start position for the image buffer
    const uint8_t* cameraStartPos = cameraBuffer;

    // Copy the data to the intermediate buffer
    int rgbPos = 0;
    for (int row = rowTop; row <= rowBottom; row++) {
        for (int col = colLeft; col <= colRight; col++) {
            int currentPos = (row * GetCameraWidth() + col) * camPixelBytes;

            // Combine the bytes that codifies the pixel (L.E.)
            unsigned short pixelCombined = 0;
            for (int b = (camPixelBytes - 1); b >= 0; b--) {
                uint8_t currentByte = cameraStartPos[currentPos + (1 - b)];
                pixelCombined |= (unsigned short)(currentByte << (b * 8));
            }

            // Convert RGB565 (16-bits) to RGB888 (24-bits)
            uint8_t baseRed = (pixelCombined & 0xF800) >> 11;
            uint8_t baseGreen = (pixelCombined & 0x07E0) >> 5;
            uint8_t baseBlue = (pixelCombined & 0x001F);

            uint8_t red = (baseRed << 3) | (baseRed >> 2);
            uint8_t green = (baseGreen << 2) | (baseGreen >> 4);
            uint8_t blue = (baseBlue << 3) | (baseBlue >> 2);

            // Store the Quantised values of the pixels
            outBuffer[3 * rgbPos] = red;
            outBuffer[3 * rgbPos + 1] = green;
            outBuffer[3 * rgbPos + 2] = blue;

            // Move to next pixel
            rgbPos++;
        }
    }
      Serial.print("Final position: ");
      Serial.print(rgbPos);
      Serial.print(" | ");
      Serial.println();
    return true;
}

// ----------------------
// Utility functions
// ----------------------

// Get the camera module ready
bool SetUpHardware() {

    // Serial for printing
    Serial.begin(9600);
    while (!Serial);

    // LEDs pins
    pinMode(LEDR, OUTPUT);
    digitalWrite(LEDR, HIGH);

    // Buttons and camera
    initializeShield();
    if (!Camera.begin(CAMERA_SHAPE, RGB565, 1, OV7675)) {
        return false;
    }
    return true;
}

// Waits for the button to be pressed to capture the image
void WaitForButton() {
    bool waiting = true;
    while (waiting) {
        waiting = !readShieldButton();
        delay(5);
    }
}

#endif
