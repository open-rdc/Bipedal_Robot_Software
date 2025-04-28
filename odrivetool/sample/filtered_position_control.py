import odrive
import time
import pygame
import signal
import sys

odrv0 = odrive.find_any()

# 制御モード設定
odrv0.axis0.controller.config.control_mode = odrive.utils.ControlMode.POSITION_CONTROL
odrv0.axis0.controller.config.input_mode = odrive.utils.InputMode.POS_FILTER
odrv0.axis0.requested_state = odrive.utils.AxisState.CLOSED_LOOP_CONTROL

# Pygameの初期化
pygame.init()
screen = pygame.display.set_mode((300, 100))  # ウィンドウは必要だが、使わない
pygame.display.set_caption("ODrive Position Control")

# 初期位置
current_position = 0

# Ctrl+Cなどの例外処理
def signal_handler(sig, frame):
    print('Exiting...')
    odrv0.axis0.requested_state = 1  # プログラムが途中で止まった際に設定するコード
    pygame.quit()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

# メインループ
running = True
clock = pygame.time.Clock()

while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    keys = pygame.key.get_pressed()

    if keys[pygame.K_UP]:
        current_position += 1.0
        odrv0.axis0.controller.input_pos = current_position
        print(f'Position: {current_position}')
        time.sleep(0.1)  # 連続入力を防止
    elif keys[pygame.K_DOWN]:
        current_position -= 1.0
        odrv0.axis0.controller.input_pos = current_position
        print(f'Position: {current_position}')
        time.sleep(0.1)

    clock.tick(60)  # 毎秒60回のループ制限

pygame.quit()