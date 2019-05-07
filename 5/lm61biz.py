from machine import ADC, Pin  # machineモジュールのADCクラスとPinクラスをインポート
import time                   # timeモジュールをインポート
adc = ADC(Pin(35))            # 35番ピンを制御するADCオブジェクトを生成
adc.atten(ADC.ATTN_11DB)      # 減衰率を11dBに設定
while True:
    temp = (adc.read() / 4095 * 3.3 + 0.1132 - 0.6) / 0.01  # ADコンバータの値から温度を求める
    print(temp)
    time.sleep(1)
