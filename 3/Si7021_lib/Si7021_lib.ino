#include "Adafruit_Si7021.h"

Adafruit_Si7021 sensor = Adafruit_Si7021();  // Adafruit_Si7021オブジェクトを初期化する

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    if (!sensor.begin()) {  // Si7021を初期化する
        Serial.println("Did not find Si7021 sensor!");
        while (true) ;
    }
}

void loop() {
    Serial.print("Humidity:    ");
    Serial.print(sensor.readHumidity(), 2);  // Si7021から湿度を読む
    Serial.print("\tTemperature: ");
    Serial.println(sensor.readTemperature(), 2);  // Si7021から温度を読む
    delay(1000);
}
