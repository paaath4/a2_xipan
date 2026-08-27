#include "myostasks.h"
#include "DJmotor.h"
#include "xipan_ctrl.h"

uint8_t BeepAlarmTimes = 0;

void LedWaterTask(void *argument)
{
    for (;;) Led_Water();
}

void BeepAlarmTask(void *argument)
{
    for (;;) {
        for (uint8_t i = 0; i < BeepAlarmTimes; i++) {
            BEEP_ON(); osDelay(40);
            BEEP_OFF(); osDelay(40);
        }
        BeepAlarmTimes = 0;
        osDelay(1);
    }
}

void MotorTask(void *argument)
{
    for (;;) {
        DJmotor_Func();     /* 1kHz 电机状态机 */
        osDelay(1);
    }
}

void StacteMac(void *argument)
{
    for (;;) {
        Xipan_StateMachine();   /* 主控命令 → 电机两位置 + 气泵 */
        osDelay(5);
    }
}
