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

/* USER CODE END Variables */
/* Definitions for LedWaterTask */
osThreadId_t LedWaterTaskHandle;
const osThreadAttr_t LedWaterTask_attributes = {
  .name = "LedWaterTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for BeepAlarmTask */
osThreadId_t BeepAlarmTaskHandle;
const osThreadAttr_t BeepAlarmTask_attributes = {
  .name = "BeepAlarmTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for MotorTask */
osThreadId_t MotorTaskHandle;
const osThreadAttr_t MotorTask_attributes = {
  .name = "MotorTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityHigh,
};
/* Definitions for StacteMac */
osThreadId_t StacteMacHandle;
const osThreadAttr_t StacteMac_attributes = {
  .name = "StacteMac",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityLow,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void LedWaterTask(void *argument);
void BeepAlarmTask(void *argument);
void MotorTask(void *argument);
void StacteMac(void *argument);

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
  LedWaterTaskHandle = osThreadNew(LedWaterTask, NULL, &LedWaterTask_attributes);
  BeepAlarmTaskHandle = osThreadNew(BeepAlarmTask, NULL, &BeepAlarmTask_attributes);
  MotorTaskHandle = osThreadNew(MotorTask, NULL, &MotorTask_attributes);
  StacteMacHandle = osThreadNew(StacteMac, NULL, &StacteMac_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_LedWaterTask */
/**
  * @brief  Function implementing the LedWaterTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_LedWaterTask */
__weak void LedWaterTask(void *argument)
{
  /* USER CODE BEGIN LedWaterTask */
  for(;;) { osDelay(1); }
  /* USER CODE END LedWaterTask */
}

/* USER CODE BEGIN Header_BeepAlarmTask */
/**
* @brief Function implementing the BeepAlarmTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_BeepAlarmTask */
__weak void BeepAlarmTask(void *argument)
{
  /* USER CODE BEGIN BeepAlarmTask */
  for(;;) { osDelay(1); }
  /* USER CODE END BeepAlarmTask */
}

/* USER CODE BEGIN Header_MotorTask */
/**
* @brief Function implementing the MotorTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_MotorTask */
__weak void MotorTask(void *argument)
{
  /* USER CODE BEGIN MotorTask */
  for(;;) { osDelay(1); }
  /* USER CODE END MotorTask */
}

/* USER CODE BEGIN Header_StacteMac */
/**
* @brief Function implementing the StacteMac thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StacteMac */
__weak void StacteMac(void *argument)
{
  /* USER CODE BEGIN StacteMac */
  for(;;) { osDelay(1); }
  /* USER CODE END StacteMac */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */
