#include "SG90.h"
#include <M5Stack.h>

int prevch;               // 前に制御したチャネル

void SG90::begin(int pin, int ch) {
    _ch = ch;
    ledcSetup(_ch, 50, 10);
    ledcAttachPin(pin, _ch);
    SG90::write(0);
}

void SG90::write(int angle) {
    if (prevch != _ch) {  // 前と違うチャネルを制御するときは
        delay(50);        // 50m秒待つ
    }
    prevch = _ch;
    ledcWrite(_ch, map(constrain(angle, -90, 90), -90, 90, _min, _max));
    _angle = constrain(angle, -90, 90);
    Serial.printf("%d: %d\r\n", _ch, _angle);
}

void SG90::move(int angle) {
    SG90::write(_angle + angle);
}
