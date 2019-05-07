#ifndef MCP3004_H  // 複数回同じヘッダがインクルードされてもよいようにする
#define MCP3004_H

#include <Arduino.h>
#include <SPI.h>

class MCP3004  // MCP3004クラス
{
public:
    MCP3004(uint8_t);
    virtual ~MCP3004();

    void begin(void);
    uint16_t read(uint8_t ch);
private:
    uint8_t _ss;
};

#endif // MCP3004_H
