#include "xipan_ctrl.h"
#include "can.h"
#include "dj_motor.h"
#include "Pump.h"

typedef enum { CMD_NONE = 0, CMD_SUCK, CMD_RELEASE, CMD_PUMP, CMD_RESET } cmd_t;

static volatile uint8_t g_enable = 0;   /* 0失能/1使能 */
static volatile cmd_t   g_cmd   = CMD_NONE;
static volatile uint8_t g_pump  = 0;    /* 0关/1开 */

extern void Beep_Trigger(void);   /* 蜂鸣(模式切换) */

void Xipan_Init(void)
{
    CAN_FilterTypeDef f = {0};
    f.FilterBank = 0;                    /* CAN1 用 bank 0 */
    f.FilterMode = CAN_FILTERMODE_IDMASK;
    f.FilterScale = CAN_FILTERSCALE_32BIT;
    f.FilterIdHigh = 0;  f.FilterIdLow = 0;
    f.FilterMaskIdHigh = 0;  f.FilterMaskIdLow = 0;   /* 全收, 回调内按 ExtId 分发 */
    f.FilterFIFOAssignment = CAN_RX_FIFO0;
    f.FilterActivation = ENABLE;
    f.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan1, &f);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);
}

void Xipan_OnCan(CAN_RxHeaderTypeDef *rx, uint8_t *d)
{
    if (rx->IDE != CAN_ID_EXT) return;   /* 主控命令是扩展帧 */
    switch (rx->ExtId) {
    case 0x01010501: g_enable = d[1]; break;                 /* 使能状态 */
    case 0x01010502: g_cmd = CMD_SUCK; break;                /* 取球: 去吸位 */
    case 0x01010503: g_cmd = CMD_RELEASE; break;             /* 放球: 去持球位 */
    case 0x01010504: g_pump = d[1]; g_cmd = CMD_PUMP; break; /* 气泵开关 */
    case 0x010205FF: g_cmd = CMD_RESET; break;               /* 复位 */
    default: break;
    }
}

void Xipan_StateMachine(void)
{
    if (!g_enable) {                 /* 未使能 */
        DJ_SetMode(1, DJ_Disable);
        pump_off();
        return;
    }
    switch (g_cmd) {
    case CMD_SUCK:                   /* 去取球位(取/放球处) */
        DJ_SetPos(1, PICK_POS_DEG);
        DJ_SetMode(1, DJ_Position);
        Beep_Trigger();
        break;
    case CMD_RELEASE:                /* 去持球位(默认位) */
        DJ_SetPos(1, HOLD_POS_DEG);
        DJ_SetMode(1, DJ_Position);
        Beep_Trigger();
        break;
    case CMD_PUMP:                
        if (g_pump) pump_set(); else pump_off();
        break;
    case CMD_RESET:                  /* 复位(系统软复位, 如需回初位改这里) */
        __set_FAULTMASK(1);
        NVIC_SystemReset();
        break;
    default: break;
    }
    g_cmd = CMD_NONE;
}
