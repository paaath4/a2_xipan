#include "xipan_ctrl.h"
#include "can.h"
#include "DJmotor.h"
#include "Pump.h"
#include "cmsis_os2.h"

volatile uint8_t g_enable = 0; /* 0失能/1使能 */
volatile cmd_t g_cmd = CMD_NONE;
volatile uint8_t g_pump = 0; /* 0关/1开 */
volatile uint8_t g_type = 0; /* 默认为取球，赋值为1后为取块 */

extern void Beep_Alarm(uint8_t times); /* 切换蜂鸣(HDL) */

/* 发送反馈(机构→主板): 扩展帧, 走 CAN1 */
static void Send_Feedback(uint32_t id, uint8_t d0, uint8_t d1)
{
    CAN_TxHeaderTypeDef th = {0};
    uint8_t data[8] = {0};
    th.IDE = CAN_ID_EXT;
    th.RTR = CAN_RTR_DATA;
    th.DLC = 2;
    th.ExtId = id;
    data[0] = d0;
    data[1] = d1;
    HAL_CAN_AddTxMessage(&hcan1, &th, data, NULL);
}

void Xipan_OnCan(CAN_RxHeaderTypeDef *rx, uint8_t *d)
{
    if (rx->IDE != CAN_ID_EXT)
        return; /* 主控命令是扩展帧 */

    switch (rx->ExtId)
    {

    case 0x01010501:
        g_enable = d[1];
        Send_Feedback(0x05010101, 'M', g_enable);
        break; /* 使能状态+回报 */

    case 0x01010502:
        g_cmd = CMD_SUCK;
        break; /* 取球: 去取球位+开泵 */

    case 0x01010503:
        g_cmd = CMD_HOLD;
        break; /* 放球: 去持球位 */

    case 0x01010504:
        g_cmd = CMD_RELEASE;
        break; /* 放球：去放球位 */
        
    case 0x01010505:
        g_pump = d[1];
        g_cmd = CMD_PUMP;
        break;/* 气泵开关控制 0关1开 */

    case 0x01010506:
        g_type = d[1];   /* 切换操作对象: 0球 1块 */
        break;

    case 0x010105FF:
        g_cmd = CMD_RESET;
        break; /* 复位 */

    default:
        break;
    }
}

void Xipan_StateMachine(void)
{
    if (g_cmd == CMD_RESET) 
    {   /* 复位 */
        __set_FAULTMASK(1);
        NVIC_SystemReset();
        return;
    }
    if (!g_enable)
    { /* 未使能: 停机 + 关泵 */
        DJmotor[DJ_MOTOR_IDX].MODE_Set = DJ_Disable;
        pump_off();
        return;
    }
    else DJmotor[DJ_MOTOR_IDX].MODE_Set = DJ_Position;
    switch (g_cmd)
    {

    case CMD_SUCK: /* 取球/块：去取位置+开泵  */
        DJmotor[DJ_MOTOR_IDX].valSet.angle_deg = (g_type == 0) ? PICKBall_POS_DEG : PICKCube_POS_DEG;
        DJmotor[DJ_MOTOR_IDX].MODE_Set = DJ_Position;
        pump_set();
        Send_Feedback(0x05010102, 'O', 'K');
        Beep_Alarm(1);
        break;

    case CMD_HOLD: /* 持球/块：去持位置 */
        DJmotor[DJ_MOTOR_IDX].valSet.angle_deg = (g_type == 0) ? HOLDBall_POS_DEG : HOLDCube_POS_DEG;
        DJmotor[DJ_MOTOR_IDX].MODE_Set = DJ_Position;
        Send_Feedback(0x05010103, 'O', 'K');
        Beep_Alarm(1);
        break;

    case CMD_RELEASE: /* 放球: 去放球位（也可作为取球预备位置）*/
        DJmotor[DJ_MOTOR_IDX].valSet.angle_deg = RELEASE_POS_DEG;
        DJmotor[DJ_MOTOR_IDX].MODE_Set = DJ_Position;
        Send_Feedback(0x05010104, 'O', 'K');
        break;

    case CMD_PUMP: /* 气泵控制*/
        if (g_pump)
            pump_set();
        else
            pump_off();
        Send_Feedback(0x05010105, 'R', g_pump); /* 气泵状态回报 */
        break;

    case CMD_RESET: /* 复位，注意需手动将机构置于最高位*/
        __set_FAULTMASK(1);
        NVIC_SystemReset();
        break;

    default:
        break;
    }
    g_cmd = CMD_NONE;
}
