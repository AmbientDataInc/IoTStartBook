/*
 * 60秒ごとに、MCP3004で1m秒間隔で100回サンプリングし、電流値を計算
 * 値をAmbientに送信
 */
#include <SPI.h>
#include "MCP3004.h"
#include "Ambient.h"

#define MCP3004_SS 5  // MCP3004のスレーブセレクトピン
MCP3004 mcp3004(MCP3004_SS);

#define TIMER0 0
#define SAMPLE_PERIOD 1     // サンプリング間隔(ミリ秒)
#define SAMPLE_SIZE 100     // 1ms x 100 = 100ms
#define PERIOD 5           // 測定間隔(秒)

const float rl = 51.0;      // 負荷抵抗

WiFiClient client;
Ambient ambient;

const char* ssid = "ssid";
const char* password = "password";

unsigned int channelId = 100; // AmbientのチャネルID
const char* writeKey = "writeKey"; // ライトキー

hw_timer_t * samplingTimer = NULL;
volatile int t0flag;

void IRAM_ATTR onTimer0() {  // タイマ割込み関数
    t0flag = 1;
}

float ampRead(uint8_t ch) {
    int vt;
    float amp, ampsum;
    ampsum = 0;

    timerAlarmEnable(samplingTimer);   // タイマを動かす
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        t0flag = 0;
        while (t0flag == 0) {          // タイマ割込みでt0flagが1になるのを待つ
            delay(0);
        }
        vt = mcp3004.read(ch);         // chの電圧値を読む
        amp = (float)(vt - 512) / 1024.0 * 3.3 / rl * 2000.0;  // 電圧値から電流値を計算
        ampsum += amp * amp;           // 二乗して足し込む
    }
    timerAlarmDisable(samplingTimer);  // タイマを止める

    return ((float)sqrt((double)(ampsum / SAMPLE_SIZE)));  // 平均値の平方根を返す
}

void setup(){
    Serial.begin(115200);
    while (!Serial);

    SPI.begin();                       // SPIを初期化
    mcp3004.begin();                   // MCP3004を初期化

    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.print("WiFi connected: ");
    Serial.println(WiFi.localIP());

    ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化

    samplingTimer = timerBegin(TIMER0, 80, true);  // 分周比80、1μ秒のタイマを作る
    timerAttachInterrupt(samplingTimer, &onTimer0, true);  // タイマ割込みハンドラを指定
    timerAlarmWrite(samplingTimer, SAMPLE_PERIOD * 1000, true);  // タイマ周期を設定
}

void loop() {
    unsigned long t = millis();
    float a0, a1;
    a0 = ampRead(0);
    a1 = ampRead(1);

    ambient.set(1, a0);
    ambient.set(2, a1);
    ambient.send();

    Serial.printf("%.2f, %.2f\r\n", a0, a1);
    while ((millis() - t) < PERIOD * 1000) {
        delay(0);
    }
}
