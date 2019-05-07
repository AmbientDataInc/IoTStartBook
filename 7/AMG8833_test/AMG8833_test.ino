#include <Wire.h>

#define FPSC 0x02
#define INTC 0x03
#define AVE 0x07
#define T01L 0x80

#define AMG88_ADDR 0x68 // in 7bit

void write8(int id, int reg, int data) {  // idで示されるデバイスにregとdataを書く
    Wire.beginTransmission(id);  // 送信先のI2Cアドレスを指定して送信の準備をする
    Wire.write(reg);  // regをキューイングする
    Wire.write(data);  // dataをキューイングする
    uint8_t result = Wire.endTransmission();  // キューイングしたデータを送信する
}

void dataread(int id, int reg, int *data, int datasize) {
    Wire.beginTransmission(id);  // 送信先のI2Cアドレスを指定して送信の準備をする
    Wire.write(reg);  // regをキューイングする
    Wire.endTransmission();  // キューイングしたデータを送信する

    Wire.requestFrom(id, datasize);  // データを受信する先のI2Cアドレスとバイト数を指定する
    int i = 0;
    while (Wire.available() && i < datasize) {
        data[i++] = Wire.read();  // 指定したバイト数分、データを読む
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    pinMode(21, INPUT_PULLUP);  // SDAをプルアップする
    pinMode(22, INPUT_PULLUP);  // SDAをプルアップする
    Wire.begin();
    
    write8(AMG88_ADDR, FPSC, 0x00);  // 10fps
    write8(AMG88_ADDR, INTC, 0x00);  // INT出力無効
    write8(AMG88_ADDR, 0x1F, 0x50);  // 移動平均出力モード有効(ここから)
    write8(AMG88_ADDR, 0x1F, 0x45);
    write8(AMG88_ADDR, 0x1F, 0x57);
    write8(AMG88_ADDR, AVE, 0x20);
    write8(AMG88_ADDR, 0x1F, 0x00);  // (ここまで)
}

void loop() {
    float temp[64];  // 8 x 8 の温度データ

    int sensorData[128];
    dataread(AMG88_ADDR, T01L, sensorData, 128);  // 128バイトのピクセルデータを読む
    for (int i = 0 ; i < 64 ; i++) {
        int16_t temporaryData = sensorData[i * 2 + 1] * 256 + sensorData[i * 2];  // 上位バイトと下位バイトから温度データを作る
        if(temporaryData > 0x200) {  // マイナスの場合
            temp[i] = (-temporaryData +  0xfff) * -0.25;
        } else {                     // プラスの場合 
            temp[i] = temporaryData * 0.25;
        }
    }

    for (int y = 0; y < 8; y++) {  // 8 x 8の温度データをシリアルモニタに出力する
        for (int x = 0; x < 8; x++) {
            Serial.printf("%2.1f ", temp[(8 - y - 1) * 8 + 8 - x - 1]);
        }
        Serial.println();
    }
    Serial.println("--------------------------------");
    delay(5000);
}
