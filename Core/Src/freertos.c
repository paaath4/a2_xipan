/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Pump.h"
#include "dj_motor.h"
#include "xipan_ctrl.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
static volatile uint8_t g_beep_req = 0;
/* USER CODE END Variables */
/* Definitions for Ledtask */
osThreadId_t LedtaskHandle;
const osThreadAttr_t Ledtask_attributes = {
  .name = "Ledtask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Motortask */
osThreadId_t MotortaskHandle;
const osThreadAttr_t Motortask_attributes = {
  .name = "Motortask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Statemac */
osThreadId_t StatemacHandle;
const osThreadAttr_t Statemac_attributes = {
  .name = "Statemac",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};
/* Definitions for Beeptask */
osThreadId_t BeeptaskHandle;
const osThreadAttr_t Beeptask_attributes = {
  .name = "Beeptask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void LedTask(void *argument);
void MotorTask(void *argument);
void StacteMac(void *argument);
void BeepTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Ledtask */
  LedtaskHandle = osThreadNew(LedTask, NULL, &Ledtask_attributes);

  /* creation of Motortask */
  MotortaskHandle = osThreadNew(MotorTask, NULL, &Motortask_attributes);

  /* creation of Statemac */
  StatemacHandle = osThreadNew(StacteMac, NULL, &Statemac_attributes);

  /* creation of Beeptask */
  BeeptaskHandle = osThreadNew(BeepTask, NULL, &Beeptask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_LedTask */
/**
  * @brief  Function implementing the Ledtask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_LedTask */
void LedTask(void *argument)
{
  /* USER CODE BEGIN LedTask */
  /* 正常工作: 两灯交替闪烁 */
  for(;;)
  {
    HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
    HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
    osDelay(500);
    HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
    osDelay(500);
  }
  /* USER CODE END LedTask */
}

/* USER CODE BEGIN Header_MotorTask */
/**
* @brief Function implementing the Motortask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MotorTask */
void MotorTask(void *argument)
{
  /* USER CODE BEGIN MotorTask */
  for(;;)
  {
    DJ_Func();      /* 1kHz 电机状态机 */
    osDelay(1);
  }
  /* USER CODE END MotorTask */
}

/* USER CODE BEGIN Header_StacteMac */
/**
* @brief Function implementing the Statemac thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StacteMac */
void StacteMac(void *argument)
{
  /* USER CODE BEGIN StacteMac */
  for(;;)
  {
    Xipan_StateMachine(); 
    osDelay(5);
  }
  /* USER CODE END StacteMac */
}

/* USER CODE BEGIN Header_BeepTask */
/**
* @brief Function implementing the Beeptask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BeepTask */
void BeepTask(void *argument)
{
  /* USER CODE BEGIN BeepTask */
  /* 上电时响两声 */
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
  osDelay(100);
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
  osDelay(100);
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
  osDelay(100);
  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
  osDelay(200);
  
  /* 之后检测到蜂鸣请求就响一声 */
  for(;;)
  {
    if (g_beep_req)
    {
      g_beep_req = 0;
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET);
      osDelay(100);
      HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET);
    }
    osDelay(20);
  }
  /* USER CODE END BeepTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void Beep_Trigger(void) { g_beep_req = 1; }   /* 切换状态蜂鸣 */
/* USER CODE END Application */

