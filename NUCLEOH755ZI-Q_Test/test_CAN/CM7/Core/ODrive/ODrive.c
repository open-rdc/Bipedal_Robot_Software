/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : ODrive.c
  * @brief          : FDCAN経由でのODriveモーターコントローラー通信用の実装ファイル
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
 *
 */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
/*
 * ODrive.c
 *
 * Created on: 2025/09/29
 * Author: Gemini
 *
 * Description:
 * FDCAN経由でのODriveモーターコントローラー通信用の実装ファイル。
 */

#include "ODrive.h"
#include <string.h> // memcpyを使用するため

// --- プライベート関数プロトタイプ ---
static void ODrive_SendCANMessage(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint8_t len);

/**
 * @brief  CANメッセージを組み立てて送信するプライベートヘルパー関数
 * @param  hfdcan FDCAN_HandleTypeDef構造体へのポインタ
 * @param  id メッセージの11-bit CAN ID
 * @param  data ペイロードデータ配列へのポインタ (最大8バイト)
 * @param  len ペイロードのデータ長 (バイト単位)
 * @retval None
 */
static void ODrive_SendCANMessage(FDCAN_HandleTypeDef *hfdcan, uint16_t id, uint8_t *data, uint8_t len) {
    FDCAN_TxHeaderTypeDef TxHeader;
    TxHeader.Identifier = id;
    TxHeader.IdType = FDCAN_STANDARD_ID;
    TxHeader.TxFrameType = FDCAN_DATA_FRAME;
    TxHeader.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
    TxHeader.BitRateSwitch = FDCAN_BRS_OFF;
    TxHeader.FDFormat = FDCAN_CLASSIC_CAN;
    TxHeader.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
    TxHeader.MessageMarker = 0;

    // データ長コード (DLC) を設定
    if (len <= 8) {
        TxHeader.DataLength = len << 16; // FDCAN_DLC_BYTES_x は (x << 16) と同等
    } else {
        TxHeader.DataLength = FDCAN_DLC_BYTES_8;
    }

    if (HAL_FDCAN_AddMessageToTxFifoQ(hfdcan, &TxHeader, data) != HAL_OK) {
        Error_Handler();
    }
    
    // アプリケーションによっては、バスの飽和を防ぐために短い遅延が有効な場合があるらしい
    HAL_Delay(1); 
}

/**
  * @brief  指定されたODrive軸の状態を設定
  */
void ODrive_SetAxisState(FDCAN_HandleTypeDef *hfdcan, uint8_t node_id, ODrive_AxisState state) {
    uint16_t can_id = (node_id << 5) | ODRIVE_CMD_SET_AXIS_STATE;
    uint8_t data[8] = {0};
    
    // ODriveプロトコルでは、状態は32-bit整数として送信される
    uint32_t requested_state = state;
    memcpy(data, &requested_state, sizeof(uint32_t));
    
    ODrive_SendCANMessage(hfdcan, can_id, data, 8);
}

/**
  * @brief  ODrive軸のコントローラーモードと入力モードを設定
  */
void ODrive_SetControllerModes(FDCAN_HandleTypeDef *hfdcan, uint8_t node_id, ODrive_ControlMode control_mode, ODrive_InputMode input_mode) {
    uint16_t can_id = (node_id << 5) | ODRIVE_CMD_SET_CONTROLLER_MODES;
    uint8_t data[8] = {0};
    
    // コントロールモード (32-bit整数)
    int32_t ctrl_mode_val = control_mode;
    memcpy(data, &ctrl_mode_val, sizeof(int32_t));
    
    // 入力モード (32-bit整数)
    int32_t input_mode_val = input_mode;
    memcpy(data + 4, &input_mode_val, sizeof(int32_t));
    
    ODrive_SendCANMessage(hfdcan, can_id, data, 8);
}

/**
  * @brief  ODrive軸に位置指令値を送信
  */
void ODrive_SetInputPos(FDCAN_HandleTypeDef *hfdcan, uint8_t node_id, float pos, int16_t vel_ff, int16_t torque_ff) {
    uint16_t can_id = (node_id << 5) | ODRIVE_CMD_SET_INPUT_POS;
    uint8_t data[8] = {0};
    
    // 位置 (float, 4バイト)
    memcpy(data, &pos, sizeof(float));
    
    // 速度フィードフォワード (int16, 2バイト)
    memcpy(data + 4, &vel_ff, sizeof(int16_t));
    
    // トルクフィードフォワード (int16, 2バイト)
    memcpy(data + 6, &torque_ff, sizeof(int16_t));
    
    ODrive_SendCANMessage(hfdcan, can_id, data, 8);
}