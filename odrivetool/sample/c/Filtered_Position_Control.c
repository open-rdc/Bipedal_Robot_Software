#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdlib.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <net/if.h>
#include <sys/socket.h>

#define NODE_ID 0x00
#define CMD_ID_SET_AXIS_STATE 0x007
#define CMD_ID_SET_CTRL_MODE  0x00B
#define CMD_ID_SET_INPUT_POS  0x00C

#define CAN_ID_SET_AXIS_STATE ((NODE_ID << 5) + CMD_ID_SET_AXIS_STATE)
#define CAN_ID_SET_CTRL_MODE  ((NODE_ID << 5) + CMD_ID_SET_CTRL_MODE)
#define CAN_ID_SET_INPUT_POS  ((NODE_ID << 5) + CMD_ID_SET_INPUT_POS)

#define VEL_FF_FIXED 1000  // int16 scaling (0.5 * 1000)
#define TORQUE_FF_FIXED 500  // int16 scaling (0.5 * 1000)

int sock;
struct sockaddr_can addr;
struct ifreq ifr;

// === CANメッセージ送信関数 ===
void send_can_cmd(uint16_t id, uint8_t *data, uint8_t len) {
    struct can_frame frame;
    frame.can_id = id;
    frame.can_dlc = len;
    memcpy(frame.data, data, len);

    if (write(sock, &frame, sizeof(frame)) != sizeof(frame)) {
        perror("[ERROR] CAN message failed to send");
    } else {
        printf("[CAN] Sent: ID=0x%03X Data=", id);
        for (int i = 0; i < len; i++) {
            printf("%02X ", data[i]);
        }
        printf("\n");
    }
    usleep(50000);  // 50ms delay
}

// === ポジション送信関数 ===
void send_position(float pos) {
    uint8_t data[8];
    memcpy(data, &pos, 4);  // Little-endian float to 4 bytes

    // int16 scaling for velocity and torque feed-forward
    data[4] = (VEL_FF_FIXED & 0xFF);
    data[5] = (VEL_FF_FIXED >> 8) & 0xFF;
    data[6] = (TORQUE_FF_FIXED & 0xFF);
    data[7] = (TORQUE_FF_FIXED >> 8) & 0xFF;
    
    send_can_cmd(CAN_ID_SET_INPUT_POS, data, 8);
    printf("[CAN] Sent position: %.2f (vel_ff=0.5, torque_ff=0.5)\n", pos);
}

// === 終了処理 ===
void signal_handler(int signum) {    
    // 状態をIDLE (1) に設定
    uint8_t idle_data[8] = {1, 0, 0, 0, 0, 0, 0, 0};
    send_can_cmd(CAN_ID_SET_AXIS_STATE, idle_data, 8);
    
    close(sock);
    exit(0);
}

// === メイン関数 ===
int main(void) {
    // CANソケット設定
    if ((sock = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
        perror("[ERROR] Failed to create socket");
        return 1;
    }

    strcpy(ifr.ifr_name, "can0");
    if (ioctl(sock, SIOCGIFINDEX, &ifr) < 0) {
        perror("[ERROR] Failed to get interface index");
        return 1;
    }

    addr.can_family = AF_CAN;
    addr.can_ifindex = ifr.ifr_ifindex;

    if (bind(sock, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("[ERROR] Failed to bind socket");
        return 1;
    }

    // Ctrl+Cでの終了をキャッチ
    signal(SIGINT, signal_handler);

    // 制御モード設定（Control_Mode=3, Input_Mode=3）
    uint8_t ctrl_mode_data[8] = {3, 0, 0, 0, 1, 0, 0, 0};
    send_can_cmd(CAN_ID_SET_CTRL_MODE, ctrl_mode_data, 8);
    sleep(1);

    // 状態をCLOSED_LOOP_CONTROL（8）に設定
    uint8_t axis_state_data[8] = {8, 0, 0, 0, 0, 0, 0, 0};
    send_can_cmd(CAN_ID_SET_AXIS_STATE, axis_state_data, 8);
    sleep(1);
    
    // === メインループ ===
    float positions[] = {0.0, 45.0, 90.0};
    int pos_count = sizeof(positions) / sizeof(positions[0]);

    while (1) {
        for (int i = 0; i < pos_count; i++) {
            float pos = positions[i] * (8.0f / 360.0f);
            send_position(pos);
            sleep(2);
        }
    }

    return 0;
}
