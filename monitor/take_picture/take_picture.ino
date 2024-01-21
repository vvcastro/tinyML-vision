#include "modules/camera.h"
#include "Arduino.h"

uint8_t modelBuffer[modelHeight * modelWidth * modelChannels];
bool commandRecv = false;

void setup() {

    // Initialise hardware and entities
    SetUpHardware();

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
            bool captureStatus = GetImage(modelBuffer);
            Serial.println("Frame:");
            Serial.write(modelBuffer, modelHeight * modelWidth * modelChannels);
            Serial.println();
        }
    }

}

