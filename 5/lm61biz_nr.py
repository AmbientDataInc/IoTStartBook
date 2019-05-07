from machine import ADC, Pin  # machineモジュールのADCクラスとPinクラスをインポート
import time                   # timeモジュールをインポート
adc = ADC(Pin(35))            # 35番ピンを制御するADCオブジェクトを生成
adc.atten(ADC.ATTN_11DB)      # 減衰率を11dBに設定
while True:
    val = []                  # ADC値のリストを作る
    for i in range(20):       # 複数回データを測定する
        val.append(adc.read())  # データを測定し、リストに追加
    mean_val = sum(val) / len(val)  # 平均値を求める
    temp = (mean_val / 4095 * 3.3 + 0.1132 - 0.6) / 0.01  # 温度を計算
    print(temp)
    time.sleep(1)             # 1秒待つ
