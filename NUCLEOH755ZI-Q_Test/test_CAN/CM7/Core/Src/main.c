/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
* 1. https://github.com/STMicroelectronics/STM32CubeH7/blob/master/Projects/STM32H743I-EVAL/Examples/FDCAN/FDCAN_Classic_Frame_Networking/Src/main.c#L294
*
*/
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */
/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
/* USER CODE END PTD */
/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif
#define NODE_ID_1 0x01
#define CMD_ID_SET_AXIS_STATE 0x007
#define CMD_ID_SET_CTRL_MODE  0x00B
#define CMD_ID_SET_INPUT_POS  0x00C
#define VEL_FF_FIXED 500  // int16 scaling (0.5 * 1000)
#define TORQUE_FF_FIXED 500  // int16 scaling (0.5 * 1000)

/* USER CODE END PD */
/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */
/* USER CODE END PM */
/* Private variables ---------------------------------------------------------*/
COM_InitTypeDef BspCOMInit;
__IO uint32_t BspButtonState = BUTTON_RELEASED;
FDCAN_HandleTypeDef hfdcan1; // 使用するFDCANのハンドル
FDCAN_HandleTypeDef hfdcan2;
/* USER CODE BEGIN PV */
uint8_t Num = 0x0;
FDCAN_TxHeaderTypeDef TxHeader1;
FDCAN_RxHeaderTypeDef RxHeader1;
FDCAN_TxHeaderTypeDef TxHeader2;
FDCAN_RxHeaderTypeDef RxHeader2;
uint8_t TxData1[8];
uint8_t RxData1[8];
uint8_t TxData2[8];
uint8_t RxData2[8];
/* USER CODE END PV */
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_FDCAN1_Init(void);
static void MX_FDCAN2_Init(void);
void send_Control_Mode(uint8_t node_id);
void send_CLOSED_LOOP_CONTROL(uint8_t node_id);
void send_position(uint8_t node_id, float pos);
void send_can_cmd(uint16_t id, uint8_t *data, uint8_t len);
void send_IDLE(uint8_t node_id);
/* USER CODE BEGIN PFP */
/* USER CODE END PFP */
/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* USER CODE END 0 */
/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
 /* USER CODE BEGIN 1 */
 /* USER CODE END 1 */
/* USER CODE BEGIN Boot_Mode_Sequence_0 */
 int32_t timeout;
/* USER CODE END Boot_Mode_Sequence_0 */
/* USER CODE BEGIN Boot_Mode_Sequence_1 */
 /* Wait until CPU2 boots and enters in stop mode or timeout*/
 timeout = 0xFFFF;
//  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
//  if ( timeout < 0 )
//  {
//  Error_Handler();
//  }
/* USER CODE END Boot_Mode_Sequence_1 */
 /* MCU Configuration--------------------------------------------------------*/
 /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
 HAL_Init();
 /* USER CODE BEGIN Init */
 /* USER CODE END Init */
 /* Configure the system clock */
 SystemClock_Config();
/* USER CODE BEGIN Boot_Mode_Sequence_2 */
/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
HSEM notification */
/*HW semaphore Clock enable*/
__HAL_RCC_HSEM_CLK_ENABLE();
/*Take HSEM */
HAL_HSEM_FastTake(HSEM_ID_0);
/*Release HSEM in order to notify the CPU2(CM4)*/
HAL_HSEM_Release(HSEM_ID_0,0);
/* wait until CPU2 wakes up from stop mode */
timeout = 0xFFFF;
while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
if ( timeout < 0 )
{
Error_Handler();
}
/* USER CODE END Boot_Mode_Sequence_2 */
 /* USER CODE BEGIN SysInit */
 /* USER CODE END SysInit */
 /* Initialize all configured peripherals */
 MX_GPIO_Init();
 MX_FDCAN1_Init();
 MX_FDCAN2_Init();
 /* USER CODE BEGIN 2 */
 /* USER CODE END 2 */
 /* Initialize leds */
 BSP_LED_Init(LED_GREEN);
 BSP_LED_Init(LED_YELLOW);
 BSP_LED_Init(LED_RED);
 /* Initialize USER push-button, will be used to trigger an interrupt each time it's pressed.*/
 BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
 /* Initialize COM1 port (115200, 8 bits (7-bit data + 1 stop bit), no parity */
 BspCOMInit.BaudRate   = 115200;
 BspCOMInit.WordLength = COM_WORDLENGTH_8B;
 BspCOMInit.StopBits   = COM_STOPBITS_1;
 BspCOMInit.Parity     = COM_PARITY_NONE;
 BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;
 if (BSP_COM_Init(COM1, &BspCOMInit) != BSP_ERROR_NONE)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN BSP */
 /* -- Sample board code to send message over COM1 port ---- */
 //printf("Welcome to STM32 world !\n\r");
 /* -- Sample board code to switch on leds ---- */
 BSP_LED_Off(LED_GREEN);
 BSP_LED_Off(LED_YELLOW);
 BSP_LED_Off(LED_RED);
 /* USER CODE END BSP */
 send_IDLE(NODE_ID_1);
 HAL_Delay(2000);
 send_Control_Mode(NODE_ID_1);
 HAL_Delay(2000);
 send_CLOSED_LOOP_CONTROL(NODE_ID_1);
 HAL_Delay(2000);
 float positions[] = {45.0, 90.0};
 int pos_count = sizeof(positions) / sizeof(positions[0]);
 /* Infinite loop */
 /* USER CODE BEGIN WHILE */
 while (1)
 {
   /* -- Sample board code for User push-button in interrupt mode ---- */
   if (BspButtonState == BUTTON_PRESSED)
   {
     /* Update button state */
     BspButtonState = BUTTON_RELEASED;
     /* -- Sample board code to toggle leds ---- */
     BSP_LED_Toggle(LED_GREEN);
     BSP_LED_Toggle(LED_YELLOW);
     BSP_LED_Toggle(LED_RED);
     /* ..... Perform your action ..... */
   }
   /*
   if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader1, TxData1) != HAL_OK) {
     Error_Handler();
   }
   else {
     BSP_LED_On(LED_GREEN);
     //printf("CAN1 Tx: %04x\n\r", Num);
   }
   if(HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan2, &TxHeader2, TxData2) != HAL_OK) {
     Error_Handler();
   }
   else {
     BSP_LED_On(LED_GREEN);
     //printf("CAN2 Tx: %04x\n\r", Num);
   }
   */
   for (int i = 0; i < pos_count; i++) {
     float pos = positions[i] * (8.0f / 360.0f);
     //printf("Sending position: %f\n", pos);
     send_position(NODE_ID_1, pos);
     HAL_Delay(5000);
   }

   BSP_LED_Off(LED_GREEN);
   BSP_LED_Off(LED_YELLOW);
   BSP_LED_Off(LED_RED);
   /* USER CODE END WHILE */
   /* USER CODE BEGIN 3 */
 }
 /* USER CODE END 3 */
}
/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
 RCC_OscInitTypeDef RCC_OscInitStruct = {0};
 RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
 /** Supply configuration update enable
 */
 HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
 /** Configure the main internal regulator output voltage
 */
 __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
 while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
 /** Initializes the RCC Oscillators according to the specified parameters
 * in the RCC_OscInitTypeDef structure.
 */
 RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
 RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
 RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
 RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
 RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
 RCC_OscInitStruct.PLL.PLLM = 4;
 RCC_OscInitStruct.PLL.PLLN = 15;
 RCC_OscInitStruct.PLL.PLLP = 2;
 RCC_OscInitStruct.PLL.PLLQ = 8;
 RCC_OscInitStruct.PLL.PLLR = 2;
 RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
 RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
 RCC_OscInitStruct.PLL.PLLFRACN = 0;
 if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
 {
   Error_Handler();
 }
 /** Initializes the CPU, AHB and APB buses clocks
 */
 RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                             |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                             |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
 RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
 RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV2;
 RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
 RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
 RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
 RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
 RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;
 if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
 {
   Error_Handler();
 }
}
/**
 * @brief FDCAN1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_FDCAN1_Init(void)
{
 /* USER CODE BEGIN FDCAN1_Init 0 */
 /* USER CODE END FDCAN1_Init 0 */
 /* USER CODE BEGIN FDCAN1_Init 1 */
 /* USER CODE END FDCAN1_Init 1 */
 hfdcan1.Instance = FDCAN1;
 hfdcan1.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
 hfdcan1.Init.Mode = FDCAN_MODE_NORMAL;
 hfdcan1.Init.AutoRetransmission = DISABLE;
 hfdcan1.Init.TransmitPause = DISABLE;
 hfdcan1.Init.ProtocolException = DISABLE;
 hfdcan1.Init.NominalPrescaler = 3;
 hfdcan1.Init.NominalSyncJumpWidth = 1;
 hfdcan1.Init.NominalTimeSeg1 = 15;
 hfdcan1.Init.NominalTimeSeg2 = 4;
 hfdcan1.Init.DataPrescaler = 1;
 hfdcan1.Init.DataSyncJumpWidth = 1;
 hfdcan1.Init.DataTimeSeg1 = 1;
 hfdcan1.Init.DataTimeSeg2 = 1;
 hfdcan1.Init.MessageRAMOffset = 0;
 hfdcan1.Init.StdFiltersNbr = 1;
 hfdcan1.Init.ExtFiltersNbr = 0;
 hfdcan1.Init.RxFifo0ElmtsNbr = 16;
 hfdcan1.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
 hfdcan1.Init.RxFifo1ElmtsNbr = 0;
 hfdcan1.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
 hfdcan1.Init.RxBuffersNbr = 0;
 hfdcan1.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
 hfdcan1.Init.TxEventsNbr = 0;
 hfdcan1.Init.TxBuffersNbr = 0;
 hfdcan1.Init.TxFifoQueueElmtsNbr = 1;
 hfdcan1.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
 hfdcan1.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
 if (HAL_FDCAN_Init(&hfdcan1) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN FDCAN1_Init 2 */
 FDCAN_FilterTypeDef sFilterConfig;
 sFilterConfig.IdType = FDCAN_STANDARD_ID;
 sFilterConfig.FilterIndex = 0;
 sFilterConfig.FilterType = FDCAN_FILTER_MASK;
 sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
 sFilterConfig.FilterID1 = 0x000;
 sFilterConfig.FilterID2 = 0x000;
 if(HAL_FDCAN_ConfigFilter(&hfdcan1, &sFilterConfig) != HAL_OK) Error_Handler();
 //HAL_FDCAN_ConfigGlobalFilter(&hfdcan1, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
 if(HAL_FDCAN_ConfigInterruptLines(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, FDCAN_INTERRUPT_LINE0))
 {
   //printf("Failed Setting Interrupt Line 0");
   Error_Handler();
 }
 if(HAL_FDCAN_Start(&hfdcan1) != HAL_OK) Error_Handler();
 if(HAL_FDCAN_ActivateNotification(&hfdcan1, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK) Error_Handler();
 TxHeader1.Identifier = 0x007; // 送信するID GIM8108-8だとCANID=0で007 デフォルトは321
 TxHeader1.IdType = FDCAN_STANDARD_ID;
 TxHeader1.TxFrameType = FDCAN_DATA_FRAME;
 TxHeader1.DataLength = FDCAN_DLC_BYTES_8;
 TxHeader1.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
 TxHeader1.BitRateSwitch = FDCAN_BRS_OFF;
 TxHeader1.FDFormat = FDCAN_CLASSIC_CAN;
 TxHeader1.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
 TxHeader1.MessageMarker = 0;
 /* USER CODE END FDCAN1_Init 2 */
}
/**
 * @brief FDCAN2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_FDCAN2_Init(void)
{
 /* USER CODE BEGIN FDCAN2_Init 0 */
 /* USER CODE END FDCAN2_Init 0 */
 /* USER CODE BEGIN FDCAN2_Init 1 */
 /* USER CODE END FDCAN2_Init 1 */
 hfdcan2.Instance = FDCAN2;
 hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
 hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
 hfdcan2.Init.AutoRetransmission = DISABLE;
 hfdcan2.Init.TransmitPause = DISABLE;
 hfdcan2.Init.ProtocolException = DISABLE;
 hfdcan2.Init.NominalPrescaler = 3;
 hfdcan2.Init.NominalSyncJumpWidth = 1;
 hfdcan2.Init.NominalTimeSeg1 = 15;
 hfdcan2.Init.NominalTimeSeg2 = 4;
 hfdcan2.Init.DataPrescaler = 1;
 hfdcan2.Init.DataSyncJumpWidth = 1;
 hfdcan2.Init.DataTimeSeg1 = 1;
 hfdcan2.Init.DataTimeSeg2 = 1;
 hfdcan2.Init.MessageRAMOffset = 17;
 hfdcan2.Init.StdFiltersNbr = 1;
 hfdcan2.Init.ExtFiltersNbr = 0;
 hfdcan2.Init.RxFifo0ElmtsNbr = 0;
 hfdcan2.Init.RxFifo0ElmtSize = FDCAN_DATA_BYTES_8;
 hfdcan2.Init.RxFifo1ElmtsNbr = 16;
 hfdcan2.Init.RxFifo1ElmtSize = FDCAN_DATA_BYTES_8;
 hfdcan2.Init.RxBuffersNbr = 0;
 hfdcan2.Init.RxBufferSize = FDCAN_DATA_BYTES_8;
 hfdcan2.Init.TxEventsNbr = 0;
 hfdcan2.Init.TxBuffersNbr = 0;
 hfdcan2.Init.TxFifoQueueElmtsNbr = 1;
 hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
 hfdcan2.Init.TxElmtSize = FDCAN_DATA_BYTES_8;
 if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK)
 {
   Error_Handler();
 }
 /* USER CODE BEGIN FDCAN2_Init 2 */
 FDCAN_FilterTypeDef sFilterConfig;
 sFilterConfig.IdType = FDCAN_STANDARD_ID;
 sFilterConfig.FilterIndex = 0;
 sFilterConfig.FilterType = FDCAN_FILTER_MASK;
 sFilterConfig.FilterConfig = FDCAN_FILTER_TO_RXFIFO1;
 sFilterConfig.FilterID1 = 0x000;
 sFilterConfig.FilterID2 = 0x000;
 if(HAL_FDCAN_ConfigFilter(&hfdcan2, &sFilterConfig) != HAL_OK) Error_Handler();
 //HAL_FDCAN_ConfigGlobalFilter(&hfdcan2, FDCAN_REJECT, FDCAN_REJECT, FDCAN_REJECT_REMOTE, FDCAN_REJECT_REMOTE);
 // Interrupt Line の設定をしてみる（DefaultだとLine０をEnableにするので）
   // stm32h7xx_hal_fdcan.c line:4991
 if(HAL_FDCAN_ConfigInterruptLines(&hfdcan2, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, FDCAN_INTERRUPT_LINE1))
 {
   //printf("Failed Setting Interrupt Line 1");
   Error_Handler();
 }
 if(HAL_FDCAN_Start(&hfdcan2) != HAL_OK) Error_Handler();
 //SET_BIT(hfdcan2.Instance->CCCR, FDCAN_CCCR_CCE);
 if(HAL_FDCAN_ActivateNotification(&hfdcan2, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK) Error_Handler();
 TxHeader2.Identifier = 0x654;
 TxHeader2.IdType = FDCAN_STANDARD_ID;
 TxHeader2.TxFrameType = FDCAN_DATA_FRAME;
 TxHeader2.DataLength = FDCAN_DLC_BYTES_8;
 TxHeader2.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
 TxHeader2.BitRateSwitch = FDCAN_BRS_OFF;
 TxHeader2.FDFormat = FDCAN_CLASSIC_CAN;
 TxHeader2.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
 TxHeader2.MessageMarker = 0;
 /* USER CODE END FDCAN2_Init 2 */
}
/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{
 GPIO_InitTypeDef GPIO_InitStruct = {0};
 /* USER CODE BEGIN MX_GPIO_Init_1 */
 /* USER CODE END MX_GPIO_Init_1 */
 /* GPIO Ports Clock Enable */
 __HAL_RCC_GPIOC_CLK_ENABLE();
 __HAL_RCC_GPIOA_CLK_ENABLE();
 __HAL_RCC_GPIOB_CLK_ENABLE();
 __HAL_RCC_GPIOG_CLK_ENABLE();
 /*Configure GPIO pins : PC1 PC4 PC5 */
 GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
 HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
 /*Configure GPIO pins : PA1 PA2 PA7 */
 GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_7;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 /*Configure GPIO pin : PB13 */
 GPIO_InitStruct.Pin = GPIO_PIN_13;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
 HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 /*Configure GPIO pins : PA8 PA11 PA12 */
 GPIO_InitStruct.Pin = GPIO_PIN_8|GPIO_PIN_11|GPIO_PIN_12;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.Alternate = GPIO_AF10_OTG1_FS;
 HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
 /*Configure GPIO pins : PG11 PG13 */
 GPIO_InitStruct.Pin = GPIO_PIN_11|GPIO_PIN_13;
 GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
 GPIO_InitStruct.Pull = GPIO_NOPULL;
 GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
 GPIO_InitStruct.Alternate = GPIO_AF11_ETH;
 HAL_GPIO_Init(GPIOG, &GPIO_InitStruct);
 /* USER CODE BEGIN MX_GPIO_Init_2 */
 /* USER CODE END MX_GPIO_Init_2 */
}
/* USER CODE BEGIN 4 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs)  // can1, can2 で、RxFIFO0とFIFO1を使い分ける感じのほうが良いのか？
{
	//printf("CB0\n");
 // if can1
 if((RxFifo0ITs & FDCAN_IT_RX_FIFO0_NEW_MESSAGE) != RESET)
 {
	if(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO0, &RxHeader1, RxData1) != HAL_OK)
	{
	  Error_Handler();
	}
	if(RxHeader1.Identifier == 0x007) // 0x123
   {
     BSP_LED_On(LED_YELLOW);
     //Num = RxData1[0];
     //printf("CAN1 Rx: %04x\n\r", Num);
     //printf("RxData1: ");
     for(int i = 0; i < 8; i++)
     {
       //printf("%02X ", RxData1[i]);
     }
     //printf("\n\r");
   }
   else if(RxHeader1.Identifier == 0x007) // 0x007
	{
	  BSP_LED_On(LED_YELLOW);
	  //Num = RxData1[0];
	  //printf("CAN1 Rx: %04x\n\r", Num);
     //printf("RxData1: ");
     for(int i = 0; i < 8; i++)
     {
       //printf("%02X ", RxData1[i]);
     }
     //printf("\n\r");
	}
	if(HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO0_NEW_MESSAGE, 0) != HAL_OK)
	{
	  Error_Handler();
	}
 }
}
void HAL_FDCAN_RxFifo1Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo1ITs)
{
 //printf("CB1\n");
 // if can2
 if((RxFifo1ITs & FDCAN_IT_RX_FIFO1_NEW_MESSAGE) != RESET)
 {
   if(HAL_FDCAN_GetRxMessage(hfdcan, FDCAN_RX_FIFO1, &RxHeader2, RxData2) != HAL_OK)
   {
     //printf("Get CAN2 Rx Message");
     Error_Handler();
   }
   if(RxHeader2.Identifier == 0x456)
   {
     BSP_LED_On(LED_YELLOW);
     Num = RxData2[0];
     //printf("CAN2 Rx: %04x\n\r", Num);
   }
   if(HAL_FDCAN_ActivateNotification(hfdcan, FDCAN_IT_RX_FIFO1_NEW_MESSAGE, 0) != HAL_OK)
   {
     Error_Handler();
   }
 }
}
/* USER CODE END 4 */
/**
 * @brief  BSP Push Button callback
 * @param  Button Specifies the pressed button
 * @retval None
 */
void BSP_PB_Callback(Button_TypeDef Button)
{
 if (Button == BUTTON_USER)
 {
   BspButtonState = BUTTON_PRESSED;
 }
}
/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
 /* USER CODE BEGIN Error_Handler_Debug */
 /* User can add his own implementation to report the HAL error return state */
 __disable_irq();
 BSP_LED_On(LED_RED);
 // if can1
 //printf("ERROR CODE: %08lx\n\r", hfdcan1.ErrorCode);
 // if can2
 //printf("ERROR CODE: %08lx\n\r", hfdcan2.ErrorCode);
 //while (1)
 //{
 //}
 /* USER CODE END Error_Handler_Debug */
}
//CANメッセージ送信関数

void send_can_cmd(uint16_t id, uint8_t *data, uint8_t len) {
   FDCAN_TxHeaderTypeDef TxHeader1;
   TxHeader1.Identifier = id; //0x123 static void MX_FDCAN1_Init(void)で0x123に固定されているかも
   TxHeader1.IdType = FDCAN_STANDARD_ID;
   TxHeader1.TxFrameType = FDCAN_DATA_FRAME;
   TxHeader1.DataLength = FDCAN_DLC_BYTES_8;
   TxHeader1.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
   TxHeader1.BitRateSwitch = FDCAN_BRS_OFF;
   TxHeader1.FDFormat = FDCAN_CLASSIC_CAN;
   TxHeader1.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
   TxHeader1.MessageMarker = 0;
   if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader1, data) != HAL_OK) {
       Error_Handler();
   } else {
       printf("[CAN] Sent: ID=0x%03X Data=", id);
       for (int i = 0; i < len; i++) {
           printf("%02X ", data[i]);
       }
       printf("\n\r");
   }
   HAL_Delay(50);
}

// 状態をCLOSED_LOOP_CONTROLに設定
void send_CLOSED_LOOP_CONTROL(uint8_t node_id) {
 uint32_t can_id;
 can_id = ((node_id << 5) + CMD_ID_SET_AXIS_STATE);
 //uint8_t test_data[8] = {0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
 TxData1[0] = 0x08;
 TxData1[1] = 0x00;
 TxData1[2] = 0x00;
 TxData1[3] = 0x00;
 TxData1[4] = 0x00;
 TxData1[5] = 0x00;
 TxData1[6] = 0x00;
 TxData1[7] = 0x00;
 send_can_cmd(can_id, TxData1, 8); // 0x123はCANのID
}

// アイドル状態に設定
void send_IDLE(uint8_t node_id) {
 uint32_t can_id;
 can_id = ((node_id << 5) + CMD_ID_SET_AXIS_STATE);
 TxData1[0] = 0x01;
 TxData1[1] = 0x00;
 TxData1[2] = 0x00;
 TxData1[3] = 0x00;
 TxData1[4] = 0x00;
 TxData1[5] = 0x00;
 TxData1[6] = 0x00;
 TxData1[7] = 0x00;
 send_can_cmd(can_id, TxData1, 8); // 0x123はCANのID
}

// 制御モード設定（Control_Mode=3, Input_Mode=3）
void send_Control_Mode(uint8_t node_id){
 uint32_t can_id;
 can_id = ((node_id << 5) + CMD_ID_SET_CTRL_MODE);
 TxData1[0] = 0x03;
 TxData1[1] = 0x00;
 TxData1[2] = 0x00;
 TxData1[3] = 0x00;
 TxData1[4] = 0x03;
 TxData1[5] = 0x00;
 TxData1[6] = 0x00;
 TxData1[7] = 0x00;
 send_can_cmd(can_id, TxData1, 8); // 0x123はCANのID
}

void send_position(uint8_t node_id, float pos) {
 uint32_t can_id;
 can_id = ((node_id << 5) + CMD_ID_SET_INPUT_POS);
 uint8_t TxData1[8];
 memcpy(TxData1, &pos, 4);  // Little-endian float to 4 bytes
 TxData1[4] = (VEL_FF_FIXED & 0xFF);
 TxData1[5] = (VEL_FF_FIXED >> 8) & 0xFF;
 TxData1[6] = (TORQUE_FF_FIXED & 0xFF);
 TxData1[7] = (TORQUE_FF_FIXED >> 8) & 0xFF;
 send_can_cmd(can_id, TxData1, 8);
 //printf("[CAN] Sent position: %.2f (vel_ff=0.5, torque_ff=0.5)\n", pos);
}

#ifdef  USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
 /* USER CODE BEGIN 6 */
 /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
 /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

