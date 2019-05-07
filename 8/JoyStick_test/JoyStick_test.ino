#define Xpin 35
#define Ypin 34

void setup() {
    Serial.begin(115200);
    while (!Serial) ;

    pinMode(Xpin, INPUT);
    pinMode(Ypin, INPUT);
}

void loop() {
    Serial.printf("%d, %d\r\n", analogRead(Xpin), analogRead(Ypin));
    delay(1000);
}
