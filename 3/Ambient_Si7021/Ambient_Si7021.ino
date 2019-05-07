#include <WiFi.h>
#include "Adafruit_Si7021.h"
#include <Ambient.h>

const char* ssid = "ssid";
const char* password = "password";

WiFiClient client;
Ambient ambient;

unsigned int channelId = 100; // AmbientのチャネルID
const char* writeKey = "writeKey"; // ライトキー

Adafruit_Si7021 sensor = Adafruit_Si7021();  // Adafruit_Si7021オブジェクトを初期化する

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    WiFi.begin(ssid, password);  // Wi-Fiネットワークに接続する
    while (WiFi.status() != WL_CONNECTED) {  // 接続したか調べる
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi connected");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());  // ローカルIPアドレスをプリントする

    ambient.begin(channelId, writeKey, &client); // チャネルIDとライトキーを指定してAmbientの初期化

    if (!sensor.begin()) {  // Si7021を初期化する
        Serial.println("Did not find Si7021 sensor!");
        while (true) ;
    }
}

void loop() {
    float temp = sensor.readTemperature();  // Si7021から温度を読む
    float humid = sensor.readHumidity();  // Si7021から湿度を読む
    Serial.printf("temp: %.2f, humid: %.2f\r\n", temp, humid);

    ambient.set(1, temp);  // Ambientのデータ1に温度をセットする
    ambient.set(2, humid);  // データ2に湿度をセットする
    ambient.send();  // Ambientに送信する

    delay(60 * 1000);
}
