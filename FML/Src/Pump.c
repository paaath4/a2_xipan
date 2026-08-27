#include "Pump.h"

void pump_off(void)
{
    HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, GPIO_PIN_RESET);
}

void pump_init(void)
{
    pump_off();
}

void pump_set(void)
{
    HAL_GPIO_WritePin(PUMP_GPIO_Port, PUMP_Pin, GPIO_PIN_SET);
}
