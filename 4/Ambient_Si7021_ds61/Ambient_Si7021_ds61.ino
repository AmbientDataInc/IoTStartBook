/*
 * ESP32でSi7021を読み、Ambientに送り、DeepSleepする
 */
#include <WiFi.h>
#include "Adafruit_Si7021.h"
#include <Ambient.h>
#include <time.h>

#define UTC     (3600 * 0)

#define TIME_TO_SLEEP  300        /* Time ESP32 will go to sleep (in seconds) */
#define SEND_CYCLE 6

RTC_DATA_ATTR int bootCount = 0;  // リカバリメモリに割り付ける
struct SensorData {
    float temp;
    float humid;
    float vbat;
};
RTC_DATA_ATTR SensorData sensorData[SEND_CYCLE];  // リカバリメモリに割り付ける

const char* ssid = "ssid";
const char* password = "password";

WiFiClient client;
Ambient ambient;

unsigned int channelId = 100; // AmbientのチャネルID
const char* writeKey = "writeKey"; // ライトキー

Adafruit_Si7021 sensor = Adafruit_Si7021();

#define BATTERY 39  // バッテリー電圧を測るピン

time_t getSntpTime() {  // インターネットから現在時刻を取得する
    time_t t;
    struct tm *tm;

    configTime(UTC, 0, "ntp.nict.jp", "ntp.jst.mfeed.ad.jp", NULL);

    t = time(NULL);
    tm = localtime(&t);
    while ((tm->tm_year + 1900) < 2000) {
        t = time(NULL);
        tm = localtime(&t);
        delay(100);
    }
    Serial.printf("Now: %04d/%02d/%02d %02d:%02d:%02d\r\n\r\n", tm->tm_year + 1900,
        tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
    return t;
}

#define BUFSIZE 500

void sendDataToAmbient(time_t current) {
    char buffer[BUFSIZE];

    ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化

    sprintf(buffer, "{\"writeKey\":\"%s\",\"data\":[", writeKey);
    for (int i = 0; i < SEND_CYCLE; i++) {
        time_t created = current - TIME_TO_SLEEP * (SEND_CYCLE - 1 - i);
        sprintf(&buffer[strlen(buffer)], "{\"created\":%d,\"time\":1,\"d1\":%2.1f,\"d2\":%.1f,\"d3\":%.2f},",
                                            created, sensorData[i].temp, sensorData[i].humid, sensorData[i].vbat);
    }
    buffer[strlen(buffer)-1] = '\0';
    sprintf(&buffer[strlen(buffer)], "]}\r\n");
    Serial.printf("%s", buffer);

    int n = ambient.bulk_send(buffer);
    Serial.printf("sent: %d\r\n", n);
}

void setup(){
    unsigned long starttime = millis();
    Serial.begin(115200);
    while (!Serial) ;

    Serial.printf("Boot count: %d\r\n", bootCount);

    if (!sensor.begin()) {
        Serial.println("Did not find Si7021 sensor!");
        while (true) ;
    }
    pinMode(BATTERY, INPUT);  // バッテリー測定ピンをINPUTモードにする

    sensorData[bootCount].temp = sensor.readTemperature();
    sensorData[bootCount].humid = sensor.readHumidity();
    sensorData[bootCount].vbat = (analogRead(BATTERY) / 4095.0 * 3.3 + 0.1132) * 2.0;
    Serial.printf("temp: %.2f, humid: %.2f, vbat: %.2f\r\n", sensorData[bootCount].temp, sensorData[bootCount].humid, sensorData[bootCount].vbat);

    if (bootCount == (SEND_CYCLE - 1)) {
        WiFi.begin(ssid, password);  // Wi-Fiネットワークに接続する
        while (WiFi.status() != WL_CONNECTED) {  // 接続したか調べる
            delay(500);
            Serial.print(".");
        }
        Serial.println("WiFi connected");
        Serial.print("IP address: ");
        Serial.println(WiFi.localIP());  // ローカルIPアドレスをプリントする

        time_t sntptime = getSntpTime();
        sendDataToAmbient(sntptime);
        bootCount = 0;
    } else {
        bootCount++;
    }
    uint64_t sleeptime = TIME_TO_SLEEP * 1000000 - (millis() - starttime) * 1000;
    esp_deep_sleep(sleeptime);  // DeepSleepモードに移行
}

void loop(){
}
