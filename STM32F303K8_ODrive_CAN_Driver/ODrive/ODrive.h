/*
 * @file    ODrive.h
 * @author  https://github.com/open-rdc
 * @reference
 * - https://github.com/siddarthiyer/ODrive-STM32-CAN-Driver/tree/main
 * - https://blog.csdn.net/weixin_45857444/article/details/127481149
 * 
 */

#ifndef ODRIVE_ODRIVE_H_
#define ODRIVE_ODRIVE_H_

#include "main.h"
#include "CAN.h"


//COMMAND ID とりあえず全部
#define GET_VERSION                     0x000
#define HEARTBEAT               		0x001
#define ESTOP                           0x002
#define GET_ERROR                       0x003
#define RXSDO                           0x004
#define TXSDO                           0x005
#define SET_AXIS_NODE_ID				0x006
#define SET_AXIS_REQUESTED_STATE 		0x007
#define MIT_CONTROL                     0x008
#define ENCODER_ESTIMATES				0x009
#define GET_ENCODER_COUNT				0x00A
#define SET_CONTROLLER_MODE 			0x00B
#define SET_INPUT_POS					0x00C
#define SET_INPUT_VEL					0x00D
#define SET_INPUT_TORQUE				0x00E
#define SET_LIMITS						0x00F
#define START_ANTICOGGING               0x010
#define SET_TRAJ_VEL_LIMIT              0x011
#define SET_TRAJ_ACCEL_LIMITS           0x012
#define SET_TRAJ_INERTIA                0x013
#define GET_IQ							0x014
#define GET_SENSORLESS_ESIMATES         0x015
#define REBOOT              			0x016
#define GET_BUS_VOLTAGE_CURRENT			0x017
#define CLEAR_ERRORS					0x018
#define SET_POSITION_GAIN				0x01A
#define SET_VEL_GAINS					0x01B
#define GET_TORQUES 					0x01C   
#define DISABLE_CAN     				0x01D
#define SAVE_CONFIGURATION              0x01E