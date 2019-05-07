#include "SG90.h"

SG90 sg90;              // SG90のインスタンスを作る

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    sg90.begin(4, 15);  // GPIO4、チャネル15で初期化
}

void loop() {
    for (int angle = -90; angle <= 90; angle += 90) {  // 角度を-90°から90°まで90°ずつ増やす
        sg90.write(angle);                             // SG90を動かす
        delay(500);
    }
}
