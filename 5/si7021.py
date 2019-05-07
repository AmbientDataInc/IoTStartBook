from micropython import const
from machine import Pin, I2C
import struct
import time

TEMP = const(0xF3)
HUMID = const(0xF5)
_RESET = const(0xfe)

class SI7021:
    def __init__(self, i2c, addr = 0x40):
        self.i2c = i2c
        self.addr = addr
        self.i2c.writeto(self.addr, struct.pack('B', _RESET))  # センサをリセットする  ----①
        time.sleep_ms(50)  # センサがリセットされるまで待つ

    def read_data(self):  # Si7021からデータを読む  ----②
        while True:
            try:  # コマンドを送った直後はセンサが反応せず、例外になる  ----③
                data = self.i2c.readfrom(self.addr, 3)
            except OSError:  # 例外になったら、無視して再度読む
                pass
            else:
                if len(data) == 3:  # 3バイト読めたらループを抜ける
                    break
        _val, _checksum = struct.unpack('>HB', data)  # 値とチェックサムを取り出す  ----④
        return _val

    @property
    def temp(self):
        self.i2c.writeto(self.addr, struct.pack('B', TEMP))  # 温度を測る  ----⑤
        return self.read_data() * 175.72 / 65536 - 46.85  # 温度データを読み、温度を計算  ----⑥

    @property
    def humid(self):
        self.i2c.writeto(self.addr, struct.pack('B', HUMID))  # 湿度を測る  ----⑦
        return self.read_data() * 125 / 65536 - 6  # 湿度データを読み、湿度を計算  ----⑧

def main():
    i2c = I2C(scl = Pin(22), sda = Pin(21))  # I2Cオブジェクトを生成  ----⑨
    si7021 = SI7021(i2c)  # SI7021オブジェクトを生成  ----⑩

    while True:
        print(si7021.temp, si7021.humid)  # 温度と湿度を測定し、出力  ----⑪
        time.sleep(1)

if __name__ == '__main__':
    main()
