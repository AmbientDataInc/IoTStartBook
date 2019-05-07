from machine import Pin, SPI
import struct
import time
import math
import ambient # Ambientライブラリをインポート

rl = 51  # 負荷抵抗
SAMPLE_PERIOD = 1  # サンプリング間隔(ミリ秒)
SAMPLE_SIZE = 100  # サンプル数
PERIODS = 5        # 測定間隔

essid = 'ssid'
password = 'password'
channelId = 100
writeKey = 'writeKey'

def do_connect():  # Wi-Fiネットワークに接続  ----①
    import network
    wlan = network.WLAN(network.STA_IF)
    wlan.active(True)
    if not wlan.isconnected():
        print('connecting to network...')
        wlan.connect(essid, password)
        while not wlan.isconnected():
            pass
    print('network config:', wlan.ifconfig())

def _data(ch):  # MCP3004から値を読む  ----②
    ret = bytearray(3)               # 受信するデータバッファ
    cmd = bytearray(struct.pack('BBB', 0x01, (0x08 | ch) << 4, 0x00))  # 送信コマンド
    mcp3004_ss.value(0)              # デバイスを選択
    spi.write_readinto(cmd, ret)     # コマンドを送信し、応答データを読む
    mcp3004_ss.value(1)              # デバイスの選択を解除
    val = struct.unpack('>BH', ret)  # 応答からデータを取り出す
    return val[1] & 0x03FF

def ampRead(ch):  # chの電流値を測定する  ----③
    ampsum = 0
    for i in range(SAMPLE_SIZE):     # サンプル数回繰り返す
        t = time.ticks_us()          # 開始時間を記録
        vt = _data(ch)               # MCP3004から電圧値を読む
        amp = (vt - 512) / 1024 * 3.3 / rl * 2000 # 電圧値を電流値に変換
        ampsum = ampsum + amp * amp  # 二乗して足し込む
        while((time.ticks_us() - t) < SAMPLE_PERIOD * 1000):  # サンプル時間、待つ
            pass
    return math.sqrt(ampsum / SAMPLE_SIZE)  # 平均値の平方根を計算

                                     # SPIオブジェクトを生成  ----④
spi = SPI(baudrate=200000, polarity=0, phase=0, sck=Pin(18), miso=Pin(19), mosi=Pin(23))
mcp3004_ss = Pin(5, Pin.OUT)
mcp3004_ss.value(1)
do_connect()
am = ambient.Ambient(channelId, writeKey)  # チャネルIDとライトキーを指定してAmbientオブジェクトを作る

while True:
    a0 = ampRead(0)                  # チャネル0の電流値を測定する
    a1 = ampRead(1)                  # チャネル1の電流値を測定する
    print(a0, a1)
    r = am.send({'d1': a0, 'd2': a1})  # チャネル0と1の電流値をAmbientに送信する
    print('status code:', r.status_code)  # 送信のステータスコードをプリント
    time.sleep(PERIODS)
