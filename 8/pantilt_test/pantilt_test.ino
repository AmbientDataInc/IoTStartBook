#include "SG90.h"

SG90 pan;               // 水平方向を制御するSG90のインスタンス
SG90 tilt;              // 上下方向を制御するSG90のインスタンス

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    pan.begin(4, 15);  // GPIO4、チャネル15で初期化
    tilt.begin(5, 14);  // GPIO5、チャネル14で初期化
}

void loop() {
    pan.write(0);      // 水平方向、正面を向く
    tilt.write(0);     // 上下方向も正面を向く
    delay(500);

    for (int a = 0; a >= -90; a -= 10) {
        pan.write(a);   // 10°ずつ右を向く
        tilt.write(a);  // 10°ずつ上を向く
        delay(50);
    }
    delay(500);

    for (int a = -90; a <= 90; a += 10) {
        pan.write(a);   // 10°ずつ左を向く
        tilt.write(a);  // 10°ずつ下を向く
        delay(50);
    }
    delay(500);

    for (int a = 90; a >= 0; a -= 10) {
        pan.write(a);   // 10°ずつ右を向く
        tilt.write(a);  // 10°ずつ上を向く
        delay(50);
    }
    delay(1000);
}
