#ifndef XIPAN_CTRL_H
#define XIPAN_CTRL_H
#include "main.h"

/* 两个位置(宏占位, 待实测) */
#define PICK_POS_DEG     120.0f   /* 取球位(取/放球处) */
#define HOLD_POS_DEG     0.0f     /* 持球位(默认/吸住球后维持) */

void Xipan_Init(void);                                    /* CAN1 主控命令接收 */
void Xipan_OnCan(CAN_RxHeaderTypeDef *rx, uint8_t *data); /* CAN1 帧解析(回调调) */
void Xipan_StateMachine(void);                            /* 状态机(任务调) */

#endif
