#include "Beep.h"
#include "myostasks.h"
#include "cmsis_os2.h"

void Beep_Init(void)
{
    BEEP_ON(); HAL_Delay(200); BEEP_OFF(); HAL_Delay(200);
    BEEP_ON(); HAL_Delay(50);  BEEP_OFF(); HAL_Delay(50);
    BEEP_ON(); HAL_Delay(50);  BEEP_OFF(); HAL_Delay(50);
}

void Beep_Alarm(uint8_t times)
{
    BeepAlarmTimes = times;
}
