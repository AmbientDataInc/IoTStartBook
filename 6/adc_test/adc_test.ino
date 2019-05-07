/*
 * MCP3004で1m秒ごとに100回サンプリングし、値をシリアルに表示
 * サンプリング値をシリアルプロッタで確認
 */
#include <SPI.h>

#define MCP3004_SS 5  // MCP3004のスレーブセレクトピン

#define TIMER0 0
#define SAMPLE_PERIOD 1     // サンプリング間隔(ミリ秒)
#define SAMPLE_SIZE 100     // 1ms x 100 = 100ms

const float rl = 51.0;      // 負荷抵抗
short amps[SAMPLE_SIZE];

hw_timer_t * samplingTimer = NULL;
volatile int t0flag;

void IRAM_ATTR onTimer0() {  // タイマ割込み関数
    t0flag = 1;
}

uint16_t mcp3004_read(uint8_t ch) {
    byte MSB, LSB;

    SPI.beginTransaction(SPISettings(200000, MSBFIRST, SPI_MODE0));
    digitalWrite(MCP3004_SS, LOW);         // デバイスを選択
    SPI.transfer(0x01);                    // スタートビットを送る
    MSB = SPI.transfer((0x08 | ch) << 4);  // チャネルを送り、上位バイトを得る
    LSB = SPI.transfer(0x00);              // 適当なデータを送り、下位バイトを得る
    digitalWrite(MCP3004_SS, HIGH);        // デバイスの選択を解除
    SPI.endTransaction();

    return (MSB & 0x03) << 8 | LSB;
}

void setup(){
    Serial.begin(115200);
    while (!Serial);

    SPI.begin();                           // SPIを初期化
    pinMode(MCP3004_SS, OUTPUT);           // スレーブセレクトピンを出力モードに
    digitalWrite(MCP3004_SS, HIGH);        // デバイス選択を解除

    samplingTimer = timerBegin(TIMER0, 80, true);  // 分周比80、1μ秒のタイマを作る
    timerAttachInterrupt(samplingTimer, &onTimer0, true);  // タイマ割込みハンドラを指定
    timerAlarmWrite(samplingTimer, SAMPLE_PERIOD * 1000, true);  // タイマ周期を設定
}

void loop() {
    timerAlarmEnable(samplingTimer);   // タイマを動かす
    for (int i = 0; i < SAMPLE_SIZE; i++) {
        t0flag = 0;
        while (t0flag == 0) {          // タイマ割込みでt0flagが1になるのを待つ
            delay(0);
        }
        amps[i] = mcp3004_read(0);     // ch0の電圧値を読む
    }
    timerAlarmDisable(samplingTimer);  // タイマを止める

    for (int i = 0; i < SAMPLE_SIZE; i++) {
        Serial.println(amps[i]);
    }
    delay(3000);
}
