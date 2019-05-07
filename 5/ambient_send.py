from machine import Pin, I2C
import time
import si7021  # 内部ファイルシステムに置いたSi7021ライブラリをインポート
import ambient # Ambientライブラリをインポート

essid = 'ssid'
password = 'password'
channelId = 100
writeKey = 'writeKey'

def do_connect():
    import network
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('connecting to network...')
        wlan.connect(essid, password)
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())

i2c = I2C(scl = Pin(22), sda = Pin(21))  # I2Cオブジェクトを作る
si7021 = si7021.SI7021(i2c)  # SI7021オブジェクトを作る
do_connect()
am = ambient.Ambient(channelId, writeKey)

while True:
    temp = si7021.temp
    humid = si7021.humid
    print(temp, humid)  # SI7021オブジェクトを使い、温度、湿度を取得
    r = am.send({'d1': temp, 'd2': humid})
    print('status code:', r.status_code)
    time.sleep(5)
