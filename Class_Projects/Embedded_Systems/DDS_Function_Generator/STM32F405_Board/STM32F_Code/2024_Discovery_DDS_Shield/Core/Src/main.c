/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include <math.h>
#include <stdlib.h>  // for atoi
#include "myFunctions.h"

#include "stdio.h"
#include "string.h"  // for strstr

#include "lcd16x2.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

// DDS MACROS
#define SAMPLING_FREQ 			1000000
#define DDS_FREQ				42400 // Default: 20kHz
#define MAX_SAMPLES 			256

// UART MACROS
#define UART_MSG_BUFFER_SIZE 	50
#define AUX_MSG_BUFFER 			50

#define CMD_DDS_ON				"DDS:ON"
#define CMD_DDS_OFF				"DDS:OFF"
#define CMD_MODE_SINE			"MODE:SINE"
#define CMD_MODE_COSINE			"MODE:COSINE"
#define CMD_MODE_SQUARE			"MODE:SQUARE"
#define CMD_MODE_TRIANGLE		"MODE:TRIANGLE"
#define CMD_MODE_SAWTOOTH		"MODE:SAWTOOTH"
#define CMD_MODE_SAWTOOTH_REV	"MODE:SAWTOOTH_REV"
#define CMD_FREQ				"FREQ:"

// WAVEFORM MACROS
#define N_WAVEFORMS 			6

// DeBounce MACROS
#define BUTTON_DEBOUNCE_TIME 	300 //ms

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim7;

UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart3_rx;

/* USER CODE BEGIN PV */

// DDS Variables
volatile uint32_t gDDS_Freq = 2000; // 1kHz
uint16_t gSample;
uint16_t gCounter;
uint32_t gPhaseAccumulator = 0;
uint32_t gTuningWord = (pow(2, 32) * DDS_FREQ) / SAMPLING_FREQ;

// Waveform Variables
typedef enum
{
	DDS_Sine_Wave = 0,
	DDS_Cosine_Wave,
	DDS_Triangle_Wave,
	DDS_Sawtooth_Wave,
	DDS_Sawtooth_Rev_Wave,
	DDS_Square_Wave,
}WaveformType_e;

volatile WaveformType_e gWaveformType = DDS_Sine_Wave;

// UART Variables
uint8_t gTXData[UART_MSG_BUFFER_SIZE];
uint8_t gRXData[UART_MSG_BUFFER_SIZE];

char gAUX_msgBuffer[AUX_MSG_BUFFER];
uint8_t gAUX_Counter = 0;

//UART state machine
typedef enum
{
	SM_START = 0,
	SM_APP,
}Uart_StateMachine;

Uart_StateMachine myUART_State = SM_START;

// State Machine Variables
typedef enum
{
	SM_Idle = 0,
	SM_Mode,
	SM_Start_Stop,
	SM_Cursor,
	SM_Plus,
	SM_Minus,
}StateMachine_e;

volatile StateMachine_e gSM = SM_Idle;

volatile uint8_t gStart_Stop = 0;
uint8_t gCursor = 0;

uint32_t gPreviousMillis = 0;
uint32_t gCurrentMillis = 0;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM7_Init(void);
static void MX_USART3_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

void my_printf(char *msg);

void ApplicationCallback(char *cmd);

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_TIM7_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  // LCD
  // **********************
  // Initialize LCD pins
  lcd16x2_init_4bits(GPIOE, LCD_RS_Pin, LCD_E_Pin,
  		  	  	  	 GPIOE, LCD_D4_Pin, LCD_D5_Pin, LCD_D6_Pin, LCD_D7_Pin );

  // Welcome LCD Screen
  lcd16x2_printf("DDS Generator");
  // Turn Off Cursor
  lcd16x2_cursorShow(false);

  // **********************

  // UART
  // **********************
  // Welcome UART
  my_printf("DDS Generator\r\n");

  // Start UART -> DMA and receive one character at the time
  HAL_UART_Receive_DMA(&huart3, (uint8_t *)gRXData, 1);

  // **********************

  //HAL_Delay(500);  // Wait 0.5 second

  // DAC and DDS
  // **********************
  // Set default DDS Tuning Word Frequency - 1kHz
  gTuningWord = (pow(2, 32) * gDDS_Freq) / SAMPLING_FREQ;

  // Start Timer 7 that interacts with External DAC
  // This is now done by using Start Stop button
  //HAL_TIM_Base_Start_IT(&htim7);

  // **********************

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {

	  switch(gSM)
	  {
	  case SM_Idle:
		  break;

	  case SM_Mode:

		  lcd16x2_clear();
		  lcd16x2_1stLine();

		  switch(gWaveformType)
		  {
		  case DDS_Sine_Wave:
			  // Update LCD
			  lcd16x2_printf("Mode: Sine");
			  // Update UART
			  my_printf("Mode: Sine\r\n");
			  break;

		  case DDS_Cosine_Wave:
			  // Update LCD
			  lcd16x2_printf("Mode: Cosine");
			  // Update UART
			  my_printf("Mode: Cosine\r\n");
			  break;

		  case DDS_Sawtooth_Wave:
			  // Update LCD
			  lcd16x2_printf("Mode: Sawtooth");
			  // Update UART
			  my_printf("Mode: Sawtooth\r\n");
			  break;

		  case DDS_Sawtooth_Rev_Wave:
			  // Update LCD
			  lcd16x2_printf("Mode: Sawtooth");
			  lcd16x2_2ndLine();
			  lcd16x2_printf("      Reverse");
			  // Update UART
			  my_printf("Mode: Sawtooth Reverse\r\n");
			  break;

		  case DDS_Square_Wave:
			  // Update LCD
			  lcd16x2_printf("Mode: Square");
			  // Update UART
			  my_printf("Mode: Square\r\n");
			  break;

		  case DDS_Triangle_Wave:
			  // Update LCD
			  lcd16x2_printf("Mode: Triangle");
			  // Update UART
			  my_printf("Mode: Triangle\r\n");
			  break;
		  }

		  // Update State Machine
		  gSM = SM_Idle;
		  break;

	  case SM_Cursor:
		  gCursor = !gCursor;

		  if(gCursor == 1)
			  lcd16x2_cursorShow(true);
		  else
			  lcd16x2_cursorShow(false);

		  // Update State Machine
		  gSM = SM_Idle;
		  break;

	  case SM_Start_Stop:

		  lcd16x2_clear();
		  lcd16x2_1stLine();

		  if(gStart_Stop == 1)
		  {
			  // Update LCD
			  lcd16x2_printf("DDS Running...");
			  // Update UART
			  my_printf("DDS Running...\r\n");

			  // Start DAC Timer
			  HAL_TIM_Base_Start_IT(&htim7);
		  }
		  else
		  {
			  // Update LCD
			  lcd16x2_printf("DDS Stopped.");
			  // Update UART
			  my_printf("DDS Stopped.\r\n");
		  }

		  // Update State Machine
		  gSM = SM_Idle;
		  break;

	  case SM_Plus:
		  // Increment Frequency
		  gDDS_Freq++;

		  // Update DDS Tuning Word Frequency
		  gTuningWord = (pow(2, 32) * gDDS_Freq) / SAMPLING_FREQ;

		  // Update LCD
		  lcd16x2_clear();
		  lcd16x2_1stLine();
		  lcd16x2_printf("Frequency:");

		  lcd16x2_2ndLine();
		  lcd16x2_printf("%d", gDDS_Freq);

		  // Update State Machine
		  gSM = SM_Idle;
		  break;

	  case SM_Minus:
		  // Decrement Frequency
		  gDDS_Freq--;

		  // Update DDS Tuning Word Frequency
		  gTuningWord = (pow(2, 32) * gDDS_Freq) / SAMPLING_FREQ;

		  // Update LCD
		  lcd16x2_clear();
		  lcd16x2_1stLine();
		  lcd16x2_printf("Frequency:");

		  lcd16x2_2ndLine();
		  lcd16x2_printf("%d", gDDS_Freq);

		  // Update State Machine
		  gSM = SM_Idle;
		  break;
	  }

	  // Main Loop timing
	  HAL_Delay(200);

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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 336;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief TIM7 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM7_Init(void)
{

  /* USER CODE BEGIN TIM7_Init 0 */

  /* USER CODE END TIM7_Init 0 */

  TIM_MasterConfigTypeDef sMasterConfig = {0};

  /* USER CODE BEGIN TIM7_Init 1 */

  /* USER CODE END TIM7_Init 1 */
  htim7.Instance = TIM7;
  htim7.Init.Prescaler = 42 - 1;
  htim7.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim7.Init.Period = 2 - 1;
  htim7.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim7) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_UPDATE;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim7, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM7_Init 2 */

  /* USER CODE END TIM7_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);

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
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin
                          |LCD_RS_Pin|LCD_E_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOD, LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |DAC_0_Pin|DAC_1_Pin|DAC_2_Pin|DAC_3_Pin
                          |DAC_4_Pin|DAC_5_Pin|DAC_6_Pin|DAC_7_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LCD_D4_Pin LCD_D5_Pin LCD_D6_Pin LCD_D7_Pin
                           LCD_RS_Pin LCD_E_Pin */
  GPIO_InitStruct.Pin = LCD_D4_Pin|LCD_D5_Pin|LCD_D6_Pin|LCD_D7_Pin
                          |LCD_RS_Pin|LCD_E_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_EVT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SW_Minus_Pin */
  GPIO_InitStruct.Pin = SW_Minus_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SW_Minus_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SW_Plus_Pin */
  GPIO_InitStruct.Pin = SW_Plus_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SW_Plus_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : BOOT1_Pin */
  GPIO_InitStruct.Pin = BOOT1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(BOOT1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : SW_Cursor_Pin */
  GPIO_InitStruct.Pin = SW_Cursor_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(SW_Cursor_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : SW_Mode_Pin SW_Start_Stop_Pin */
  GPIO_InitStruct.Pin = SW_Mode_Pin|SW_Start_Stop_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : LD4_Pin LD3_Pin LD5_Pin LD6_Pin
                           DAC_0_Pin DAC_1_Pin DAC_2_Pin DAC_3_Pin
                           DAC_4_Pin DAC_5_Pin DAC_6_Pin DAC_7_Pin */
  GPIO_InitStruct.Pin = LD4_Pin|LD3_Pin|LD5_Pin|LD6_Pin
                          |DAC_0_Pin|DAC_1_Pin|DAC_2_Pin|DAC_3_Pin
                          |DAC_4_Pin|DAC_5_Pin|DAC_6_Pin|DAC_7_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI1_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

void my_printf(char *msg)
{
	sprintf((char *)gTXData, msg);
	HAL_UART_Transmit(&huart3, gTXData, strlen(gTXData), HAL_MAX_DELAY);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim7)
	{
		// Take the 8 upper bits of the 32 bit phase accumulator as a LUT sample pointer
		gCounter = (gPhaseAccumulator >> 24) % MAX_SAMPLES;

		// Get sample from LUT
		switch(gWaveformType)
		{
		case DDS_Sine_Wave:
			gSample = (uint16_t)(SINE_WAVE[gCounter]);
			break;

		case DDS_Cosine_Wave:
			gSample = (uint16_t)(COSINE_WAVE[gCounter]);
			break;

		case DDS_Sawtooth_Wave:
			gSample = (uint16_t)(SAWTOOTH_WAVE[gCounter]);
			break;

		case DDS_Sawtooth_Rev_Wave:
			gSample = (uint16_t)(SAWTOOTH_REV_WAVE[gCounter]);
			break;

		case DDS_Triangle_Wave:
			gSample = (uint16_t)(TRIANGLE_WAVE[gCounter]);
			break;

		case DDS_Square_Wave:
			gSample = (uint16_t)(SQUARE_WAVE[gCounter]);
			break;
		}

		// Faster alternative - direct register access
		GPIOD->ODR &= 0xFF00;  // set 8 bits to zero
		GPIOD->ODR |= (gSample & 0x00FF);  // set 8 bits to gSample

		// Increment Phase Accumulator/counter
		gPhaseAccumulator += gTuningWord;
	}
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(GPIO_Pin);

	// Get currentMillis
	gCurrentMillis = HAL_GetTick();

	if(GPIO_Pin == SW_Start_Stop_Pin && (gCurrentMillis - gPreviousMillis > BUTTON_DEBOUNCE_TIME))
	{
		// Update State Machine variable
		gSM = SM_Start_Stop;

		// Update Start Stop global variable
		gStart_Stop = !gStart_Stop;

		if(gStart_Stop == 1)
		{
			// Start DAC Timer
			// This is done in the Main Loop
			//HAL_TIM_Base_Start_IT(&htim7);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
		}
		else
		{
			// Stop DAC Timer
			HAL_TIM_Base_Stop_IT(&htim7);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
		}

		// Update previousMillis
		gPreviousMillis = gCurrentMillis;
	}

	if(GPIO_Pin == SW_Mode_Pin && (gCurrentMillis - gPreviousMillis > BUTTON_DEBOUNCE_TIME))
	{
		// Update previousMillis
		gPreviousMillis = gCurrentMillis;

		// Update State Machine variable
		gSM = SM_Mode;

		// Rotate between waveforms
		gWaveformType = (gWaveformType + 1) % N_WAVEFORMS;
	}

	if(GPIO_Pin == SW_Cursor_Pin && (gCurrentMillis - gPreviousMillis > BUTTON_DEBOUNCE_TIME))
	{
		// Update previousMillis
		gPreviousMillis = gCurrentMillis;

		// Update State Machine variable
		gSM = SM_Cursor;
	}

	if(GPIO_Pin == SW_Plus_Pin && (gCurrentMillis - gPreviousMillis > BUTTON_DEBOUNCE_TIME))
	{
		// Update previousMillis
		gPreviousMillis = gCurrentMillis;

		// Update State Machine variable
		gSM = SM_Plus;
	}

	if(GPIO_Pin == SW_Minus_Pin && (gCurrentMillis - gPreviousMillis > BUTTON_DEBOUNCE_TIME))
	{
		// Update previousMillis
		gPreviousMillis = gCurrentMillis;

		// Update State Machine variable
		gSM = SM_Minus;
	}

}

// UART DMA Complete Callback Function
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	/* Prevent unused argument(s) compilation warning */
	UNUSED(huart);

	// State Machine
	// ****************

	switch(myUART_State)
	{
	case SM_START:

		// Sync character detected
		if(gRXData[0] == '#')
		{
			// Go to next state to receive the remaining commands
			myUART_State = SM_APP;

			// Reset gAUX_Counter
			gAUX_Counter = 0;
		}

		else
		{
			if( (gRXData[0] != '\r') || (gRXData[0] != '\n') )
			{
				my_printf("Invalid command.\r\n");
			}
		}

		// Restart DMA reception
		HAL_UART_Receive_DMA(&huart3, (uint8_t *)gRXData, 1);

		break;

	case SM_APP:

		// Get end char
		if(gRXData[0] == '*')
		{

			// ApplicationCallback
			ApplicationCallback(gAUX_msgBuffer);

			// Go back to sync state
			myUART_State = SM_START;
		}
		else
		{
			// add char to gAUX_msgBuffer
			gAUX_msgBuffer[gAUX_Counter] = gRXData[0];

			// increment counter
			gAUX_Counter++;
		}

		// Restart DMA reception
		HAL_UART_Receive_DMA(&huart3, (uint8_t *)gRXData, 1);

		break;

	default:
		break;
	}

	// ****************
}

void ApplicationCallback(char *cmd)
{
	// DDS Command
	// *****************
	if( strstr(cmd, "DDS") != NULL )
	{
		if( strstr(cmd, CMD_DDS_ON) != NULL )
		{
			// Update State Machine variable
			gSM = SM_Start_Stop;

			// Update Start Stop global variable
			gStart_Stop = 1;

			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);

			my_printf("CMD Ok.\r\n");
		}

		if( strstr(cmd, CMD_DDS_OFF) != NULL )
		{
			// Update State Machine variable
			gSM = SM_Start_Stop;

			// Update Start Stop global variable
			gStart_Stop = 0;

			// Stop DAC Timer
			HAL_TIM_Base_Stop_IT(&htim7);
			HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);

			my_printf("CMD Ok.\r\n");
		}
	}
	// *****************

	// MODE Command
	// *****************
	if( strstr(cmd, "MODE") != NULL )
	{
		// SINE WAVEFORM
		if( strstr(cmd, CMD_MODE_SINE) != NULL )
		{
			// Update State Machine variable
			gSM = SM_Mode;

			gWaveformType = DDS_Sine_Wave;

			my_printf("CMD Ok.\r\n");
		}

		// COSINE WAVEFORM
		if( strstr(cmd, CMD_MODE_COSINE) != NULL )
		{
			// Update State Machine variable
			gSM = SM_Mode;

			gWaveformType = DDS_Cosine_Wave;

			my_printf("CMD Ok.\r\n");
		}

		// TRIANGLE WAVEFORM
		if( strstr(cmd, CMD_MODE_TRIANGLE) != NULL )
		{
			// Update State Machine variable
			gSM = SM_Mode;

			gWaveformType = DDS_Triangle_Wave;

			my_printf("CMD Ok.\r\n");
		}

		// SQUARE WAVEFORM
		if( strstr(cmd, CMD_MODE_SQUARE) != NULL )
		{
			// Update State Machine variable
			gSM = SM_Mode;

			gWaveformType = DDS_Square_Wave;

			my_printf("CMD Ok.\r\n");
		}

		// SAWTOOTH WAVEFORM
		if( strstr(cmd, CMD_MODE_SAWTOOTH) != NULL )
		{
			// Update State Machine variable
			gSM = SM_Mode;

			gWaveformType = DDS_Sawtooth_Wave;

			my_printf("CMD Ok.\r\n");
		}

		// SAWTOOTH REVERSE WAVEFORM
		if( strstr(cmd, CMD_MODE_SAWTOOTH_REV) != NULL )
		{
			// Update State Machine variable
			gSM = SM_Mode;

			gWaveformType = DDS_Sawtooth_Rev_Wave;

			my_printf("CMD Ok.\r\n");
		}
	}
	// *****************

	// FREQ Command
	// *****************
	if( strstr(cmd, "FREQ") != NULL )
	{
		// Get pointer
		const char* freqPos = strstr(cmd, "FREQ:");
		if(freqPos != NULL)
		{
			// Move the pointer to the position just after "FREQ:"
			freqPos += strlen("FREQ:");
			// Extract the number using atoi
			gDDS_Freq = (uint32_t) atoi(freqPos);

			// Update DDS Tuning Word Frequency
			gTuningWord = (pow(2, 32) * gDDS_Freq) / SAMPLING_FREQ;

			my_printf("CMD Ok.\r\n");
		}
	}
	// *****************

}

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
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
