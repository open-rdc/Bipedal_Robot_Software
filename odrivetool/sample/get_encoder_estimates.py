import odrive
import pygame
import signal
import sys

odrv0 = odrive.find_any()

# エンコーダの値を取得 odrivetool
# https://docs.odriverobotics.com/v/latest/guides/odrive_package/odrive.utils.html

# Ctrl+Cなどの例外処理
def signal_handler(sig, frame):
    print('Exiting...')
    odrv0.axis0.requested_state = 1  # プログラムが途中で止まった際に設定するコード
    pygame.quit()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

while True:
    print("--------------------")
    #print(f'encoder.pos_estimate:{odrv0.axis0.encoder.pos_estimate}')
    #print(f'encoder.vel_estimate:{odrv0.axis0.encoder.vel_estimate}')
    #print(f'encoder.shadow_count:{odrv0.axis0.encoder.shadow_count}')
    #print(f'encoder.count_in_cpr:{odrv0.axis0.encoder.count_in_cpr}')
    print(f"角度:{360/8*odrv0.axis0.encoder.pos_estimate}")
    #print(f"角速度:{360/8*odrv0.axis0.encoder.vel_estimate/60}")
