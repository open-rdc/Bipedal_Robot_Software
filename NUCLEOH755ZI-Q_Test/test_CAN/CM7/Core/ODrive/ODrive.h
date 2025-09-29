/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : ODrive.h
  * @brief          : FDCAN経由でのODriveモーターコントローラー通信用ヘッダーファイル
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/**
 * Reference Sites:
 * 1. https://github.com/siddarthiyer/ODrive-STM32-CAN-Driver/tree/main/ODrive
 * 2. https://docs.odriverobotics.com/v/latest/manual/can-protocol.html#can-protocol
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/

/*
 * ODrive.h
 *
 * Created on: 2025/09/29
 * Author: Gemini
 *
 * Description:
 * FDCAN経由でのODriveモーターコントローラー通信用ヘッダーファイル。
 * このドライバはSTM32 HALライブラリ向けに調整されています。
 */

#ifndef INC_ODRIVE_H_
#define INC_ODRIVE_H_

#include "main.h" // FDCAN_HandleTypeDefを含むSTM32 HALの定義をインクルード

// --- ODrive コマンドID ---
#define ODRIVE_CMD_SET_AXIS_STATE       0x007
#define ODRIVE_CMD_SET_CONTROLLER_MODES 0x00B
#define ODRIVE_CMD_SET_INPUT_POS        0x00C

// --- ODrive 軸状態 ---
typedef enum {
    ODRIVE_AXIS_STATE_IDLE                    = 0x01,
    ODRIVE_AXIS_STATE_CLOSED_LOOP_CONTROL     = 0x08
} ODrive_AxisState;

// --- ODrive コントロールモード ---
typedef enum {
    ODRIVE_CONTROL_MODE_VOLTAGE_CONTROL = 0x0,
    ODRIVE_CONTROL_MODE_TORQUE_CONTROL  = 0x1,
    ODRIVE_CONTROL_MODE_VELOCITY_CONTROL = 0x2,
    ODRIVE_CONTROL_MODE_POSITION_CONTROL = 0x3
} ODrive_ControlMode;

// --- ODrive 入力モード ---
typedef enum {
    ODRIVE_INPUT_MODE_INACTIVE      = 0x0,
    ODRIVE_INPUT_MODE_PASSTHROUGH   = 0x1,
    ODRIVE_INPUT_MODE_VEL_RAMP      = 0x2,
    ODRIVE_INPUT_MODE_POS_FILTER    = 0x3,
    ODRIVE_INPUT_MODE_TRAP_TRAJ     = 0x5,
    ODRIVE_INPUT_MODE_TORQUE_RAMP   = 0x6
} ODrive_InputMode;


/**
  * @brief  指定されたODrive軸の状態を設定
  * @param  hfdcan FDCAN_HandleTypeDef構造体へのポインタ
  * @param  node_id ターゲットとなるODrive軸のCANノードID
  * @param  state 要求する軸の状態 (例: ODRIVE_AXIS_STATE_IDLE)
  * @retval None
  */
void ODrive_SetAxisState(FDCAN_HandleTypeDef *hfdcan, uint8_t node_id, ODrive_AxisState state);

/**
  * @brief  ODrive軸のコントローラーモードと入力モードを設定
  * @param  hfdcan FDCAN_HandleTypeDef構造体へのポインタ
  * @param  node_id ターゲットとなるODrive軸のCANノードID
  * @param  control_mode 要求するコントロールモード (例: ODRIVE_CONTROL_MODE_POSITION_CONTROL)
  * @param  input_mode 要求する入力モード (例: ODRIVE_INPUT_MODE_POS_FILTER)
  * @retval None
  */
void ODrive_SetControllerModes(FDCAN_HandleTypeDef *hfdcan, uint8_t node_id, ODrive_ControlMode control_mode, ODrive_InputMode input_mode);

/**
  * @brief  ODrive軸に位置指令値を送信
  * @param  hfdcan FDCAN_HandleTypeDef構造体へのポインタ
  * @param  node_id ターゲットとなるODrive軸のCANノードID
  * @param  pos 要求する位置 [turns]
  * @param  vel_ff 速度フィードフォワード項 (スケール済み整数)
  * @param  torque_ff トルクフィードフォワード項 (スケール済み整数)
  * @retval None
  */
void ODrive_SetInputPos(FDCAN_HandleTypeDef *hfdcan, uint8_t node_id, float pos, int16_t vel_ff, int16_t torque_ff);

#endif /* INC_ODRIVE_H_ */