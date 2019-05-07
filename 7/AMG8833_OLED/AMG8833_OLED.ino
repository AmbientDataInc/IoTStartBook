#include <Wire.h>
#include "AMG8833.h"
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1351.h>

#define AMG88_ADDR 0x68 // I2Cアドレス

AMG8833 amg8833(AMG88_ADDR);

#define SCREEN_WIDTH  128
#define SCREEN_HEIGHT 96 // Change this to 96 for 1.27" OLED.

#define SCLK_PIN 18
#define MOSI_PIN 23
#define DC_PIN   16
#define CS_PIN   5
#define RST_PIN  4

#define    BLACK           0x0000

Adafruit_SSD1351 tft = Adafruit_SSD1351(SCREEN_WIDTH, SCREEN_HEIGHT, CS_PIN, DC_PIN, MOSI_PIN, SCLK_PIN, RST_PIN);  

uint32_t heat(float);           // heat関数の宣言

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    pinMode(21, INPUT_PULLUP);  // SDAをプルアップする
    pinMode(22, INPUT_PULLUP);  // SDAをプルアップする
    Wire.begin();               // Wireライブラリを初期化
    amg8833.begin();            // AMG8833を初期化

    tft.begin();                // 液晶画面を初期化
    tft.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, BLACK);  // 液晶画面を黒く塗りつぶす
}

#define CELL_W (SCREEN_WIDTH / 8)
#define CELL_H (SCREEN_HEIGHT / 8)

void loop() {
    float temp[64];  // AMG8833から読んだ8 x 8の温度データ

    amg8833.read(temp);

    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            float t = temp[(8 - y - 1) * 8 + 8 - x - 1];
            uint16_t color = heat(map(constrain((int)t, 0, 60), 0, 60, 0, 100) / 100.0);
            tft.fillRect(x * CELL_W, y * CELL_H, CELL_W, CELL_H, color);
        }
    }
}
