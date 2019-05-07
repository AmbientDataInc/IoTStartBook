#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Wire.h>
#include "AMG8833.h"          // 赤外線アレイセンサのヘッダファイル
#include "SG90.h"

#define AMG88_ADDR 0x68       // 赤外線アレイセンサのI2Cアドレス

AMG8833 amg8833(AMG88_ADDR);  // 赤外線アレイセンサのインスタンスを生成

SG90 pan;                     // 水平方向を制御するSG90のインスタンス
SG90 tilt;                    // 上下方向を制御するSG90のインスタンス

const char *ssid = "ssid";
const char *password = "password";

WebServer server(80);

void handleRoot() {
    String msg = "hello";  // レスポンスメッセージを用意
    Serial.println(msg);
    server.send(200, "text/plain", msg);  // レスポンスを返信する
}

void handleCapture() {  // /captureをアクセスされたときの処理
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

void handleStream() {  // /streamをアクセスされたときの処理
    char buf[400];

    snprintf(buf, 400,  // thermal.svgというファイルをアクセスするHTMLを作る
   "<html>\
    <head>\
        <meta http-equiv='refresh' content='0.5'/>\
    </head>\
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
    server.on("/capture", handleCapture);  // /captureの処理関数を登録
    server.on("/stream", handleStream);  // /streamの処理関数を登録
    server.on("/thermal.svg", handleThermal);  // /thermal.svgの処理関数を登録
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("access: http://thermoCam.local/capture for still image");
    Serial.println("access: http://thermoCam.local/stream for stream image");

    pan.begin(4, 15);  // GPIO4、チャネル15で初期化
    tilt.begin(5, 14);  // GPIO5、チャネル14で初期化

    pan.write(0);      // 水平方向、正面を向く
    tilt.write(0);     // 上下方向も正面を向く
    delay(500);
}

const float Pi = atan(1) * 4;
#define d2r(d) ((d) / 180.0 * Pi)
#define r2d(r) ((r) * 180.0 / Pi)
const float l = 4.0 / tan(d2r(30.0));

float temp[64];
unsigned long lastT = 0;

void loop(void) {
    float maxT = 0.0;       // 最高温度
    int Tx[64], Ty[64];     // 最高温度のx、y座標
    int Tn = 0;             // 最高温度の個数
    float Cx, Cy;           // 最高温度の重心座標

    server.handleClient();

    if ((millis() - lastT) > 500) {  // 500m秒に1回処理する
        lastT = millis();

        amg8833.read(temp);  // AMG8833から温度データを取得

        for (int y = 0; y < 8; y++) {
            for (int x = 0; x < 8; x++) {
                float t = temp[(8 - y - 1) * 8 + 8 - x - 1];  // 画素(x, y)の温度を取り出す
                if (maxT < t) {          // 最高温度が更新されたら
                    maxT = t;
                    Tn = 1;              // 最高温度の個数を1にして、
                    Tx[Tn - 1] = x;      // 最高温度の座標を記録
                    Ty[Tn - 1] = y;
                } else if (maxT == t) {  // 最高温度と同じ温度が見つかったら
                    Tn++;                // 最高温度の個数を1増やして、
                    Tx[Tn - 1] = x;      // 最高温度の座標を記録
                    Ty[Tn - 1] = y;
                }
            }
        }
        Cx = Cy = 0.0;
        for (int i = 0; i < Tn; i++) {
            Cx += Tx[i];
            Cy += Ty[i];
        }
        Cx = Cx / Tn - 4.0;  // 中心座標が(0, 0)になるようにして重心座標を計算
        Cy = Cy / Tn - 4.0;

        Serial.printf("center: (%.1f, %.1f, %.1f, %.1f)\r\n", Cx, Cy, r2d(atan(Cx / l)), r2d(atan(Cy / l)));
        if (Cx) {
            pan.move(-1 * r2d(atan(Cx / l)));
        }
        if (Cy) {
            tilt.move(r2d(atan(Cy / l)));
        }
    }
}

void handleThermal() {  // /thermal.svgの処理関数
    String out = "";
    char buf[100];
    out += "<svg xmlns=\"http://www.w3.org/2000/svg\" version=\"1.1\">\n";
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            float t = temp[(8 - y - 1) * 8 + 8 - x - 1];  // 画素(x, y)の温度を取り出す
            uint32_t color = heat(map(constrain((int)t, 0, 60), 0, 60, 0, 100) / 100.0);
//            Serial.printf("%2.1f ", t);
            sprintf(buf, "<rect x=\"%d\" y=\"%d\" width=\"50\" height=\"50\" fill=\"#%06x\" />\n",
                x * 50, y * 50, color);
            out += buf;
        }
//        Serial.println();
    }
    Serial.println("--------------------------------");
    out += "</svg>\n";

    server.send(200, "image/svg+xml", out);
}
