#ifndef XIPAN_CTRL_H
#define XIPAN_CTRL_H
#include "main.h"

/* 两个位置(待实测) */
#define PICK_POS_DEG     120.0f   /* 取球位(取球/放球处) */
#define HOLD_POS_DEG     0.0f     /* 持球位(默认/吸住球后维持) */

void Xipan_Init(void);                                    /* 主控命令接收 */
void Xipan_OnCan(CAN_RxHeaderTypeDef *rx, uint8_t *data); /* 帧解析 */
void Xipan_StateMachine(void);                            /* 状态机 */

#endif
