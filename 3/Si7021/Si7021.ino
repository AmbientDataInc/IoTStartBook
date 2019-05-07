#include <Wire.h>

#define SI7021_ADDR 0x40

#define SI7021_TEMP_HOLD        0xE3 // 温度を読む, Hold Master Mode
#define SI7021_TEMP_NOHOLD      0xF3 // 温度を読む, No Hold Master Mode
#define SI7021_RH_HOLD          0xE5 // 湿度を読む, Hold Master Mode
#define SI7021_RH_NOHOLD        0xF5 // 湿度を読む, No Hold Master Mode
#define SI7021_RESET            0xFE

void reset(void) {
    Wire.beginTransmission(SI7021_ADDR);  // コマンド送信先のI2Cアドレスを指定する
    Wire.write(SI7021_RESET);  // 送信するコマンドを準備する
    Wire.endTransmission();  // 準備したコマンドを送信する
    delay(50);
}

float readHumidity(void) {
    Wire.beginTransmission(SI7021_ADDR);
    Wire.write(SI7021_RH_NOHOLD);
    Wire.endTransmission();

    while (true) {
        if (Wire.requestFrom(SI7021_ADDR, 3) == 3) {
            uint16_t hum = Wire.read() << 8 | Wire.read();
            uint8_t chxsum = Wire.read();
            return (float)hum * 125 / 65536 - 6;
        }
        delay(6);
    }
}

float readTemperature(void) {
    Wire.beginTransmission(SI7021_ADDR);
    Wire.write(SI7021_TEMP_NOHOLD);
    Wire.endTransmission();

    while (true) {
        if (Wire.requestFrom(SI7021_ADDR, 3) == 3) {
            uint16_t temp = Wire.read() << 8 | Wire.read();
            uint8_t chxsum = Wire.read();
            return (float)temp * 175.72 / 65536 - 46.85;
        }
        delay(6);
    }
}

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    Wire.begin();
    reset();
}

void loop() {
    float temp = readTemperature();
    float humid = readHumidity();
    Serial.printf("temp; %.2f, humid: %.2f\r\n", temp, humid);
    delay(1000);
}
