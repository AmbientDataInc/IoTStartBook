#include <WiFi.h>        // 必要なヘッダファイルをインクルードする
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>

WebServer server(80);   // ポート番号を指定してWebServerのインスタンスを作る

const char *ssid = "ssid";
const char *password = "password";

void serverRoot() {
    String msg = "hello";  // レスポンスメッセージを用意
    Serial.println(msg);
    server.send(200, "text/plain", msg);  // レスポンスを返信する
}

void handleNotFound(){
  String msg = "";
  server.send(404, "text/plain", "");
}

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    WiFi.begin(ssid, password);    // Wi-Fiネットワークに接続
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    Serial.print("\r\nWiFi connected: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", HTTP_GET, serverRoot);  // URLを指定して、処理する関数を登録する
    server.onNotFound(handleNotFound);     // 指定していないURLへのアクセスを処理する関数を登録する
    server.begin();                        // Webサーバを起動する
    Serial.println("access: http://esp32.local");
}

void loop() {
  server.handleClient();  // クライアントからのリクエストを処理する
}
