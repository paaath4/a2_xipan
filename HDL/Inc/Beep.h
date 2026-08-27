#ifndef BEEP_H
#define BEEP_H
#include "main.h"
#include "gpio.h"

#define BEEP_ON()  HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_SET)
#define BEEP_OFF() HAL_GPIO_WritePin(BEEP_GPIO_Port, BEEP_Pin, GPIO_PIN_RESET)

void Beep_Init(void);           /* 上电双响 */
void Beep_Alarm(uint8_t times); /* 设蜂鸣次数, 任务响应 */

#endif
