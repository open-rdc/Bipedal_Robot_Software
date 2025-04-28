import can
import time
import pygame
import struct
import signal
import sys

# === 設定 ===
#NODE_ID = 0x00  # ドライバのNode ID（必要に応じて変更） ID = 0
NODE_ID_0 = 0x00
NODE_ID_1 = 0x01
NODE_ID_2 = 0x02
CMD_ID_SET_AXIS_STATE = 0x007
CMD_ID_SET_CTRL_MODE = 0x00B
CMD_ID_SET_INPUT_POS = 0x00C

CAN_ID_SET_AXIS_STATE_0 = (NODE_ID_0 << 5) + CMD_ID_SET_AXIS_STATE
CAN_ID_SET_CTRL_MODE_0 = (NODE_ID_0 << 5) + CMD_ID_SET_CTRL_MODE
CAN_ID_SET_INPUT_POS_0 = (NODE_ID_0 << 5) + CMD_ID_SET_INPUT_POS

CAN_ID_SET_AXIS_STATE_1 = (NODE_ID_1 << 5) + CMD_ID_SET_AXIS_STATE
CAN_ID_SET_CTRL_MODE_1 = (NODE_ID_1 << 5) + CMD_ID_SET_CTRL_MODE
CAN_ID_SET_INPUT_POS_1 = (NODE_ID_1 << 5) + CMD_ID_SET_INPUT_POS

CAN_ID_SET_AXIS_STATE_2 = (NODE_ID_2 << 5) + CMD_ID_SET_AXIS_STATE
CAN_ID_SET_CTRL_MODE_2 = (NODE_ID_2 << 5) + CMD_ID_SET_CTRL_MODE
CAN_ID_SET_INPUT_POS_2 = (NODE_ID_2 << 5) + CMD_ID_SET_INPUT_POS

# === CANインタフェース ===
bus = can.interface.Bus(channel='can0', interface='socketcan')  # 'can0' は正しいIF名に変更

# === 初期化関数 ===
def send_can_cmd(arbitration_id, data_bytes):
    msg = can.Message(arbitration_id=arbitration_id, data=data_bytes, is_extended_id=False)
    bus.send(msg)
    print(msg)
    time.sleep(0.05)

# 制御モード設定（Control_Mode=3, Input_Mode=3）
send_can_cmd(CAN_ID_SET_CTRL_MODE_0, [3, 0, 0, 0, 3, 0, 0, 0])
send_can_cmd(CAN_ID_SET_CTRL_MODE_1, [3, 0, 0, 0, 3, 0, 0, 0])
send_can_cmd(CAN_ID_SET_CTRL_MODE_2, [3, 0, 0, 0, 3, 0, 0, 0])
print("[CAN] Sent: Set_Controller_Mode -> Position, Filtered Input")
time.sleep(1.0)

# 状態をCLOSED_LOOP_CONTROL（8）に設定
send_can_cmd(CAN_ID_SET_AXIS_STATE_0, [8, 0, 0, 0, 0, 0, 0, 0])
send_can_cmd(CAN_ID_SET_AXIS_STATE_1, [8, 0, 0, 0, 0, 0, 0, 0])
send_can_cmd(CAN_ID_SET_AXIS_STATE_2, [8, 0, 0, 0, 0, 0, 0, 0])
print("[CAN] Sent: Set_Axis_State -> CLOSED_LOOP_CONTROL (8)")  
time.sleep(1.0)

# === Pygame初期化 ===
pygame.init()
screen = pygame.display.set_mode((300, 100))
pygame.display.set_caption("GIM8108-8 Position Control")

# Ctrl+Cなどの例外処理
def signal_handler(sig, frame):
    print('Exiting...')
    # odrv0.axis0.requested_state = 1
    # プログラムが途中で止まった際に設定するコード
    send_can_cmd(CAN_ID_SET_AXIS_STATE_0, [1, 0, 0, 0, 0, 0, 0, 0])
    send_can_cmd(CAN_ID_SET_AXIS_STATE_1, [1, 0, 0, 0, 0, 0, 0, 0])
    send_can_cmd(CAN_ID_SET_AXIS_STATE_2, [1, 0, 0, 0, 0, 0, 0, 0])
    pygame.quit()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

current_position = 0.0
running = True
clock = pygame.time.Clock()

# 固定値：0.5をint16スケーリング（例: ×1000 → 500）
VEL_FF_FIXED = 500
TORQUE_FF_FIXED = 500

def send_position(pos):
    pos_bytes = struct.pack('<f', pos)
    vel_bytes = struct.pack('<h', VEL_FF_FIXED)
    torque_bytes = struct.pack('<h', TORQUE_FF_FIXED)
    data = list(pos_bytes + vel_bytes + torque_bytes)
    msg_0 = can.Message(arbitration_id=CAN_ID_SET_INPUT_POS_0, data=data, is_extended_id=False)
    msg_1 = can.Message(arbitration_id=CAN_ID_SET_INPUT_POS_1, data=data, is_extended_id=False)
    msg_2 = can.Message(arbitration_id=CAN_ID_SET_INPUT_POS_2, data=data, is_extended_id=False)
    bus.send(msg_0)
    bus.send(msg_1)
    bus.send(msg_2)
    print(f"[CAN] Sent position: {pos:.2f} (vel_ff=0.5, torque_ff=0.5)")

# === メインループ ===
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    keys = pygame.key.get_pressed()

    if keys[pygame.K_UP]:
        current_position += 1.0
        send_position(current_position)
        time.sleep(0.1)

    elif keys[pygame.K_DOWN]:
        current_position -= 1.0
        send_position(current_position)
        time.sleep(0.1)

    clock.tick(60)

pygame.quit()