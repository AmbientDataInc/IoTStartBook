#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include "AMG8833.h"          // 赤外線アレイセンサのヘッダファイル  ----①

#define AMG88_ADDR 0x68       // 赤外線アレイセンサのI2Cアドレス

AMG8833 amg8833(AMG88_ADDR);  // 赤外線アレイセンサのインスタンスを生成

const char *ssid = "ssid";
const char *password = "password";

WebServer server(80);

void handleRoot() {
    String msg = "hello";  // レスポンスメッセージを用意
    Serial.println(msg);
    server.send(200, "text/plain", msg);  // レスポンスを返信する
}

void handleCapture() {  // /captureをアクセスされたときの処理  ----②
    char buf[400];

    snprintf(buf, 400,  // thermal.svgというファイルをアクセスするHTMLを作る
   "<html>\
    <body>\
        <div align=\"center\">\
            <img src=\"/thermal.svg\" width=\"400\" height=\"400\" />\
        </div>\
    </body>\
    </html>"
          );
    server.send(200, "text/html", buf);  // HTMLを返信する
}

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";

    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }

    server.send(404, "text/plain", message);
}

uint32_t heat(float);           // heat関数の宣言

void setup(void) {
    Serial.begin(115200);

    pinMode(21, INPUT_PULLUP);  // AMG8833をアクセスする準備
    pinMode(22, INPUT_PULLUP);
    Wire.begin();
    amg8833.begin();

    WiFi.mode(WIFI_STA);
    Serial.println("");

    // Wait for connection
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.print("Connected to ");
    Serial.println(ssid);
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    if (MDNS.begin("thermoCam")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", handleRoot);
    server.on("/capture", handleCapture);  // /captureの処理関数を登録  ----④
    server.on("/thermal.svg", handleThermal);  // /thermal.svgの処理関数を登録
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("access: http://thermoCam.local/capture");
}

float temp[64];
unsigned long lastT = 0;

void loop(void) {
    server.handleClient();

    if ((millis() - lastT) > 500) {
        lastT = millis();

        amg8833.read(temp);  // AMG8833から温度データを取得
    }
}

void handleThermal() {  // /thermal.svgの処理関数  ----⑤
    String out = "";
    char buf[100];  // AMG8833の画素データを
    out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            float t = temp[(8 - y - 1) * 8 + 8 - x - 1];
            uint32_t color = heat(map(constrain((int)t, 0, 60), 0, 60, 0, 100) / 100.0);
            Serial.printf("%2.1f ", t);
            sprintf(buf, "<rect x=\"%d\" y=\"%d\" width=\"50\" height=\"50\" fill=\"#%06x\" />\n",
                x * 50, y * 50, color);
            out += buf;
        }
        Serial.println();
    }
    Serial.println("--------------------------------");
    out += "</svg>\n";

    server.send(200, "image/svg+xml", out);
}
