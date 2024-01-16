#include "modules/camera.h"

uint8_t modelBuffer[modelHeight * modelWidth * modelChannels];

void setup() {

    // Initialise hardware and entities
    SetUpHardware();

}

void loop() {

    // Waits until the button is clicked
    WaitForButton();

    bool captureStatus = GetImage(modelBuffer);

    Serial.println("Frame:");
    Serial.write(modelBuffer, modelHeight * modelWidth * modelChannels);
    Serial.println();
}

