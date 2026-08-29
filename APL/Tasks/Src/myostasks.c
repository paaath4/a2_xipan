#include "myostasks.h"
#include "DJmotor.h"
#include "xipan_ctrl.h"

uint8_t BeepAlarmTimes = 0;

void LedTask(void *argument)
{
    for (;;) Led_Water();
}

void BeepTask(void *argument)
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
// 调试代码
//	DJmotor[DJ_MOTOR_IDX].Begin = true;
//  DJmotor[DJ_MOTOR_IDX].MODE_Set = DJ_Position;
//  DJmotor[DJ_MOTOR_IDX].valSet.angle_deg = DJmotor[1].valNow.angle_deg; 
    for (;;) {
        Xipan_StateMachine();   /* 主控命令 → 电机两位置 + 气泵 */
        osDelay(5);
			}
    
}
