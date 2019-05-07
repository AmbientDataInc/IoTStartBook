#include <Wire.h>
#include "AMG8833.h"

AMG8833::AMG8833(uint8_t addr) {
    _addr = addr;
}

AMG8833::~AMG8833() {
}

void AMG8833::begin(void) {
    _write8(_addr, FPSC, 0x00);  // 10fps
    _write8(_addr, INTC, 0x00);  // INT出力無効
    _write8(_addr, 0x1F, 0x50);  // 移動平均出力モード有効
    _write8(_addr, 0x1F, 0x45);
    _write8(_addr, 0x1F, 0x57);
    _write8(_addr, AVE, 0x20);
    _write8(_addr, 0x1F, 0x00);
}

void AMG8833::read(float * temp) {
    int sensorData[128];

    _dataread(_addr, T01L, sensorData, 128);
    for (int i = 0 ; i < 64 ; i++) {
        int16_t temporaryData = sensorData[i * 2 + 1] * 256 + sensorData[i * 2];
        if(temporaryData > 0x200) {
            temp[i] = (-temporaryData +  0xfff) * -0.25;
        } else {
            temp[i] = temporaryData * 0.25;
        }
    }
}

void AMG8833::_write8(int id, int reg, int data) {
    Wire.beginTransmission(id);
    Wire.write(reg);
    Wire.write(data);
    uint8_t result = Wire.endTransmission();
}

void AMG8833::_dataread(int id,int reg,int *data,int datasize) {
    Wire.beginTransmission(id);
    Wire.write(reg);
    Wire.endTransmission();

    Wire.requestFrom(id, datasize);
    int i = 0;
    while (Wire.available() && i < datasize) {
        data[i++] = Wire.read();
    }
}
