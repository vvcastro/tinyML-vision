#include "tensorflow/lite/micro/micro_log.h"
#include "tensorflow/lite/micro/micro_utils.h"

#include "modules/camera.h"
#include "TinyMLShield.h"

#ifndef ARDUINO_EXCLUDE_CODE

#include "Arduino.h"

const COLOR_NAME COLOR_SCHEME = RGB_COLOR;

namespace {

    // Allocate tensors one to optimise memory
    constexpr size_t bytesPerPixel = GetPixelBytes(COLOR_SCHEME);
    uint8_t rawImageBuffer[rawCols * rawRows * bytesPerPixel];
    uint8_t croppedImageBuffer[modelCols * modelRows * bytesPerPixel];
    uint8_t rgbImageBuffer[modelCols * modelRows * 3];

    // Capture a frame and store it into the buffer
    TfLiteStatus CaptureFrame() {
        Camera.readFrame(rawImageBuffer);
        return kTfLiteOk;
    }

    // Crop the image to the required input image size
    TfLiteStatus CropFrameImage() {

        // Define the vertix for the crop
        const size_t rowTop = (rawRows - modelRows) / 2;
        const size_t rowBottom = rowTop + modelRows - 1;
        const size_t colLeft = (rawCols - modelCols) / 2;
        const size_t colRight = colLeft + modelCols - 1;

        // Place the pointer at the start of the image ()
        const size_t bytesPixel = (const size_t)Camera.bytesPerPixel();
        const uint8_t* cropStart = rawImageBuffer; // + ((verTop * rawCols) + horLeft) * multiplier;

        // Copy the data to the intermediate buffer
        int idx = 0;
        for (size_t row = rowTop; row <= rowBottom; row++) {
            size_t rowPos = row * rawCols * bytesPixel;
            for (size_t col = colLeft; col <= colRight; col++) {
                for (size_t b = 0; b < bytesPixel; b++, idx++) {
                    croppedImageBuffer[idx] = cropStart[rowPos + col * bytesPixel + b];
                }
            }
        }
        return kTfLiteOk;
    }

    // Takes the cropped image and transform it to RGB (3-channels) for the model
    TfLiteStatus ConvertToRGB() {
        const size_t frameShape = modelCols * modelRows;

        // Copy the data to the intermediate buffer
        int idx = 0;
        for (int pos = 0; pos < frameShape; pos++) {

            // Combine the bytes that codifies the pixel (L.E.)
            unsigned short pixelCombined = 0;
            for (int b = (bytesPerPixel - 1); b >= 0; b--, idx++) {
                uint8_t currentByte = croppedImageBuffer[idx];
                pixelCombined |= (unsigned short)(currentByte << (b * 8));
            }

            // Convert RGB565 (16-bits) to RGB888 (24-bits)
            uint8_t baseRed = (pixelCombined & 0xF800) >> 11;
            uint8_t baseGreen = (pixelCombined & 0x07E0) >> 5;
            uint8_t baseBlue = (pixelCombined & 0x001F);

            uint8_t red = (baseRed << 3) | (baseRed >> 2);
            uint8_t green = (baseGreen << 2) | (baseGreen >> 4);
            uint8_t blue = (baseBlue << 3) | (baseBlue >> 2);

            // Store in the buffer
            rgbImageBuffer[3 * pos] = red;
            rgbImageBuffer[3 * pos + 1] = green;
            rgbImageBuffer[3 * pos + 2] = blue;
        }

        return kTfLiteOk;
    }

    // Qunatisises the image given the training algo used
    TfLiteStatus QuantizeFrameImage(int8_t* outputBuffer) {
        int imageSize = (modelRows * modelCols * GetPixelBytes(COLOR_SCHEME));
        for (int idx = 0; idx <= imageSize; idx++) {
            outputBuffer[idx] = static_cast<int8_t>(croppedImageBuffer[idx] - 128);
        }
        return kTfLiteOk;
    }
}

// We assume the camera is ready for adquisitions
TfLiteStatus GetImage(int8_t* imgBuffer) {
    MicroPrintf("Camera: Starting frame acquisitions...");

    // (1) Read a frame from the camera
    MicroPrintf(" - Camera: Capturing frame...");
    TfLiteStatus captureStatus = CaptureFrame();
    if (captureStatus != kTfLiteOk) {
        MicroPrintf(" - Error: Frame captured failed!");
        return captureStatus;
    }

    // (2) Crop/Resize the image to the desired input shape
    MicroPrintf(" - Camera: Cropping frame...");
    TfLiteStatus cropStatus = CropFrameImage();
    if (cropStatus != kTfLiteOk) {
        MicroPrintf(" - Error: Frame crop failed!");
        return cropStatus;
    }

    // (3) Transform to RGB space
    MicroPrintf(" - Camera: Transforming to RGB...");
    TfLiteStatus rgbStatus = ConvertToRGB();
    if (rgbStatus != kTfLiteOk) {
        MicroPrintf(" - Error: Conversion to RGB failed!");
        return rgbStatus;
    }
    Serial.write(rgbImageBuffer, modelCols * modelRows * 3);
    Serial.println();

    // (4) Quantize input image for model prediction
    // MicroPrintf(" - Camera: Quantisizing frame...");
    // TfLiteStatus quantStatus = QuantizeFrameImage(imgBuffer);
    // if (quantStatus != kTfLiteOk) {
    //     MicroPrintf(" - Error: Quantize Image failed");
    //     return quantStatus;
    // }

    MicroPrintf(" - Success: Frame acquired!");
    return kTfLiteOk;
}

// ----------------------
// Utility functions
// ----------------------

// Get the camera module ready
TfLiteStatus SetUpHardware() {

    // Serial for printing
    Serial.begin(9600);
    while (!Serial);

    // LEDs pins
    pinMode(LEDR, OUTPUT);
    digitalWrite(LEDR, HIGH);

    // Buttons and camera
    initializeShield();
    if (!Camera.begin(QCIF, COLOR_SCHEME, 5, OV7675)) {
        MicroPrintf("Camera (Error): Initialisation failed!");
        return kTfLiteError;
    }
    MicroPrintf("Camera: OV7675 is ready to capture!");
    return kTfLiteOk;
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
