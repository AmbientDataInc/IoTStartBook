#include "MCP3004.h"

MCP3004::MCP3004(uint8_t ss) {
    _ss = ss;
}

MCP3004::~MCP3004() {
}

void MCP3004::begin(void) {
    pinMode(_ss, OUTPUT);                  // スレーブセレクトピンを出力モードに
    digitalWrite(_ss, HIGH);               // デバイス選択を解除
}

uint16_t MCP3004::read(uint8_t ch) {
    byte MSB, LSB;

    SPI.beginTransaction(SPISettings(200000, MSBFIRST, SPI_MODE0));
    digitalWrite(_ss, LOW);                // デバイスを選択
    SPI.transfer(0x01);                    // スタートビットを送る
    MSB = SPI.transfer((0x08 | ch) << 4);  // チャネルを送り、上位バイトを得る
    LSB = SPI.transfer(0x00);              // 適当なデータを送り、下位バイトを得る
    digitalWrite(_ss, HIGH);               // デバイスの選択を解除
    SPI.endTransaction();

    return (MSB & 0x03) << 8 | LSB;
}
