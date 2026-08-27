#include "xipan_ctrl.h"
#include "can.h"
#include "DJmotor.h"
#include "Pump.h"

typedef enum { CMD_NONE = 0, CMD_SUCK, CMD_RELEASE, CMD_PUMP, CMD_RESET } cmd_t;

static volatile uint8_t g_enable = 0;   /* 0失能/1使能 */
static volatile cmd_t   g_cmd   = CMD_NONE;
static volatile uint8_t g_pump  = 0;    /* 0关/1开 */

extern void Beep_Alarm(uint8_t times);   /* 切换蜂鸣(HDL) */


void Xipan_OnCan(CAN_RxHeaderTypeDef *rx, uint8_t *d)
{
    if (rx->IDE != CAN_ID_EXT) return;   /* 主控命令是扩展帧 */
    switch (rx->ExtId) {
    case 0x01010501: g_enable = d[1]; break;                 /* 使能状态 */
    case 0x01010502: g_cmd = CMD_SUCK; break;                /* 取球: 去取球位 */
    case 0x01010503: g_cmd = CMD_RELEASE; break;             /* 放球: 去持球位 */
    case 0x01010504: g_pump = d[1]; g_cmd = CMD_PUMP; break; /* 气泵开关 */
    case 0x010205FF: g_cmd = CMD_RESET; break;               /* 复位 */
    default: break;
    }
}

void Xipan_StateMachine(void)
{
    if (!g_enable) {                       /* 未使能: 停机 + 关泵 */
        DJmotor[0].MODE_Set = DJ_Disable;
        pump_off();
        return;
    }
    switch (g_cmd) {
    case CMD_SUCK:                         /* 去取球位(取/放球处) */
        DJmotor[0].valSet.angle_deg = PICK_POS_DEG;
        DJmotor[0].MODE_Set = DJ_Position;
        Beep_Alarm(1);
        break;
    case CMD_RELEASE:                      /* 去持球位(默认位) */
        DJmotor[0].valSet.angle_deg = HOLD_POS_DEG;
        DJmotor[0].MODE_Set = DJ_Position;
        Beep_Alarm(1);
        break;
    case CMD_PUMP:                         /* 气泵独立 */
        if (g_pump) pump_set(); else pump_off();
        break;
    case CMD_RESET:                        /* 复位(系统软复位) */
        __set_FAULTMASK(1);
        NVIC_SystemReset();
        break;
    default: break;
    }
    g_cmd = CMD_NONE;
}
