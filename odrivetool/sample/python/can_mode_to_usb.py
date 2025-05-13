# 参考
# https://zenn.dev/nexis_r/articles/4697e25ca87c76?redirected=1

import can

def send_can_message(bus, can_id, data):
    message = can.Message(arbitration_id=can_id, data=data, is_extended_id=False)
    try:
        bus.send(message)
        print(f"Message sent on {bus.channel_info}")
    except can.CanError as e:
        print(f"Message NOT sent: {e}")

if __name__ == "__main__":
    # CANバスを初期化
    bus = can.interface.Bus(channel='can0', interface='socketcan')

    try:
        # node_idとcmd_idを設定
        node_id = 0x00  # 使用するノードIDを指定 0x?? -> ?の部分はノードIDの番号を記載する
        cmd_id = 0x01E  # Disable_CANのコマンドID
        can_id = (node_id << 5) | cmd_id

        # データは空の8バイト（コマンド送信のみでデータなし）
        data = [0x00] * 8

        # CANメッセージを送信
        send_can_message(bus, can_id, data)
    finally:
        # バスをシャットダウン
        bus.shutdown()