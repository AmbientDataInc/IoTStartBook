import time  # timeモジュールをインポート  ----①
start = time.ticks_ms()  # プログラムの開始時刻を記録
SLEEP_TIME = 10
from machine import Pin, I2C
import machine
import si7021  # 内部ファイルシステムに置いたSi7021ライブラリをインポート
import ambient # Ambientライブラリをインポート

essid = 'ssid'
password = 'password'
channelId = 100
writeKey = 'writeKey'

def do_connect():  # Wi-Fiネットワークに接続
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
am = ambient.Ambient(channelId, writeKey)  # チャネルIDとライトキーを指定してAmbientオブジェクトを作る

temp = si7021.temp  # Si7021から温度を取得  ----②
humid = si7021.humid  # 湿度を取得
print(temp, humid)
r = am.send({'d1': temp, 'd2': humid})  # 温度、湿度をAmbientに送信
print('status code:', r.status_code)  # 送信のステータスコードをプリント

machine.deepsleep(SLEEP_TIME * 1000 - (time.ticks_ms() - start))  # Deep sleepする  ----③
