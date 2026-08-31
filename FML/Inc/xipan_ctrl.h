#ifndef XIPAN_CTRL_H
#define XIPAN_CTRL_H
#include "main.h"

/* 两个位置(宏占位, 待实测) */
#define PICK_POS_DEG     -125.0f   /* 取球位(取/放球处)  */
#define HOLD_POS_DEG     -50.0f    /* 持球位(默认/吸住球后维持) */
#define RELEASE_POS_DEG  -95.0f   /* 放球位置  */

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
typedef enum 
{ CMD_NONE    = 0,
  CMD_SUCK    = 1, 
  CMD_HOLD    = 2, 
  CMD_RELEASE = 3,
  CMD_PUMP    = 4,
  CMD_RESET   = 5
 
} cmd_t;

extern volatile uint8_t g_enable;  //是否使能
extern volatile cmd_t   g_cmd;     //功能命令关键字（0:默认状态     1:取球(开泵)    2:持球   3:放球    4:气泵开关      5复位）
extern volatile uint8_t g_pump;    //气泵开关关键字

void Xipan_OnCan(CAN_RxHeaderTypeDef *rx, uint8_t *data); /* CAN1 帧解析(回调调) */
void Xipan_StateMachine(void);                            /* 状态机(任务调) */

#endif
