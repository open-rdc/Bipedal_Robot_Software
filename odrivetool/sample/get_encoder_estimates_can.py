import can
import time
import pygame
import struct
import signal
import sys

# === 設定 ===
NODE_ID = 0x02
CMD_ID_SET_AXIS_STATE = 0x007
CMD_ID_SET_CTRL_MODE = 0x00B
CMD_ID_SET_INPUT_POS = 0x00C
CMD_ID_GET_ENCODER_ESTIMATES = 0x09  # Get Encoder Estimates

CAN_ID_SET_AXIS_STATE = (NODE_ID << 5) + CMD_ID_SET_AXIS_STATE
CAN_ID_SET_CTRL_MODE = (NODE_ID << 5) + CMD_ID_SET_CTRL_MODE
CAN_ID_SET_INPUT_POS = (NODE_ID << 5) + CMD_ID_SET_INPUT_POS
CAN_ID_GET_ENCODER_ESTIMATES = (NODE_ID << 5) + CMD_ID_GET_ENCODER_ESTIMATES

# CANインターフェースを設定
bus = can.interface.Bus("can0", bustype="socketcan")

# === 初期化関数 ===
def send_can_cmd(arbitration_id, data_bytes):
    msg = can.Message(arbitration_id=arbitration_id, data=data_bytes, is_extended_id=False)
    bus.send(msg)
    print(msg)
    time.sleep(0.05)

def get_encoder_estimates():
    # エンコーダのデータをリクエスト
    bus.send(can.Message(arbitration_id=CAN_ID_GET_ENCODER_ESTIMATES, data=[0, 0, 0, 0, 0, 0, 0, 0], is_extended_id=False))
    for msg in bus:
        if msg.arbitration_id == CAN_ID_GET_ENCODER_ESTIMATES:
            pos, vel = struct.unpack('<ff', bytes(msg.data))
            return pos, vel
    return None, None

# Ctrl+Cなどの例外処理
def signal_handler(sig, frame):
    print('Exiting...')
    send_can_cmd(CAN_ID_SET_AXIS_STATE, [1, 0, 0, 0, 0, 0, 0, 0])
    pygame.quit()
    sys.exit(0)

signal.signal(signal.SIGINT, signal_handler)

while True:
    # エンコーダの値を取得して表示
    pos, vel = get_encoder_estimates()
    if pos is not None:
        # print(f"Encoder position: {pos:.3f} turns, velocity: {vel:.3f} turns/s")
        print(f"Encoder position: {pos:.3f} turns, velocity: {vel:.3f} turns/s")
        print(f"Encoder position: {pos*(360/8):.3f} °, velocity: {vel*(360/8):.3f} °/s")
