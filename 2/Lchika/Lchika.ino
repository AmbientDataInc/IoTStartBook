#define LED 25  // LEDという文字列を25と定義する

void setup() {  // 最初に1回だけ実行される
    pinMode(LED, OUTPUT);  // LEDピンのモードを出力モードにする
}

void loop() {  // 繰り返し実行される
    digitalWrite(LED, HIGH);  // LEDピンをHIGH(3.3V)にする
    delay(500);               // 0.5秒待つ
    digitalWrite(LED, LOW);   // LEDピンをLOW(0V)にする
    delay(500);               // 0.5秒待つ
}
