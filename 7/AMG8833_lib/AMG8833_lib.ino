#include <Wire.h>
#include "AMG8833.h"

#define AMG88_ADDR 0x68 // I2Cアドレス

AMG8833 amg8833(AMG88_ADDR);

void setup() {
    Serial.begin(115200);
    pinMode(21, INPUT_PULLUP);
    pinMode(22, INPUT_PULLUP);
    Wire.begin();
    amg8833.begin();
}

void loop() {
    float temp[64];

    amg8833.read(temp);

    int x, y;
    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            float t = temp[(8 - y - 1) * 8 + 8 - x - 1];
            Serial.printf("%2.1f ", t);
        }
        Serial.println();
    }
    Serial.println("--------------------------------");
    delay(500);
}
