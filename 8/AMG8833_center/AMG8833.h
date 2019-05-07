#ifndef AMG8833_H
#define AMG8833_H

#include <Arduino.h>

#define FPSC 0x02
#define INTC 0x03
#define AVE 0x07
#define T01L 0x80

class AMG8833 { // AMG8833クラス
public:
    AMG8833(uint8_t);
    virtual ~AMG8833();

    void begin(void);
    void read(float *);
private:
    uint8_t _addr;
    void _write8(int, int, int);
    void _dataread(int, int, int *, int);
};

#endif // AMG8833_h
