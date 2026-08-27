#include "Led.h"
#include "cmsis_os2.h"

void Led_Water(void)
{
    LED_ON(1); LED_OFF(2); osDelay(500);
    LED_OFF(1); LED_ON(2); osDelay(500);
}
