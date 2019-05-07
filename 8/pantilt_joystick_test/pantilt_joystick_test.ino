#include "SG90.h"

#define Xpin 35        // X方向のジョイスティックの値を読むピン
#define Ypin 34        // Y方向のジョイスティックの値を読むピン

SG90 pan;               // 水平方向を制御するSG90のインスタンス
SG90 tilt;              // 上下方向を制御するSG90のインスタンス

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    pan.begin(4, 15);  // GPIO4、チャネル15で初期化
    tilt.begin(5, 14);  // GPIO5、チャネル14で初期化

    pan.write(0);      // 水平方向、正面を向く
    tilt.write(0);     // 上下方向も正面を向く
    delay(500);

    pinMode(Xpin, INPUT);  // ジョイスティックの値を読むピンを入力モードに
    pinMode(Ypin, INPUT);
}

void loop() {
    int x = analogRead(Xpin);  // X方向のジョイスティックの値を読む
    int y = analogRead(Ypin);  // Y方向も読む

    Serial.printf("%d, %d\r\n", map(x, 0, 4095, -90, 90), map(y, 0, 4095, -90, 90));
    pan.write(map(x, 0, 4095, -90, 90));  // xの値が0〜4095に対して-90°から90°に制御
    tilt.write(map(y, 0, 4095, -90, 90));  // yも同様
    delay(500);
}
