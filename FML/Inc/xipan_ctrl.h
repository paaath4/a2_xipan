#ifndef XIPAN_CTRL_H
#define XIPAN_CTRL_H
#include "main.h"

/* 两个位置(宏占位, 待实测) */
#define PICK_POS_DEG     -10.0f   /* 取球位(取/放球处) */
#define HOLD_POS_DEG     10.0f     /* 持球位(默认/吸住球后维持) */

/*初始时刻将吸盘机构置于最高处，已经测得最高处与最低处相距约为170
几个位置：
初始位置（最高点）
最低位置（最低点）
前往取球位（较低点）
前往持球点（较高点）

电机的angel_deg的正值为逆时针旋转
初始位置设为0值，后续位置均为负值（顺时针）
注意一次性调节范围不要过大，容易超调导致机构相互碰撞


待完成
reset的函数完善
反馈消息的部分
错误警报部分




*/

void Xipan_OnCan(CAN_RxHeaderTypeDef *rx, uint8_t *data); /* CAN1 帧解析(回调调) */
void Xipan_StateMachine(void);                            /* 状态机(任务调) */

#endif
