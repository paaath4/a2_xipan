#ifndef _XIPAN_PID_H_
#define _XIPAN_PID_H_

#include "main.h"

/* PID 计算方式 */
typedef enum
{
    PID_POSITION = 0,   /* 位置式: 输出绝对量 */
    PID_INCREMENT = 1   /* 增量式: Δ输出自累加 */
} pidmode_t;

typedef struct
{
    float setval;        /* 设定值 */
    float curval;        /* 反馈值 */
    float kp;            /* 比例增益 */
    float ki;            /* 积分增益 */
    float kd;            /* 微分增益 */
    float integral;      /* 积分累计 */
    float last_e1;       /* 上次误差 */
    float last_e2;       /* 上上次误差 */
    float max_out;       /* 输出限幅(±) */
    float max_integral;  /* 积分限幅(±), 抗饱和 */
    uint8_t enabled;     /* 1 = 参与计算 */
    pidmode_t mode;      /* 计算方式 */
} pidtype;

void pid_init(pidtype *p, float kp, float ki, float kd, pidmode_t mode);  /* 设增益与方式 */
void pid_reset(pidtype *p);                                               /* 清零累积量 */
float pid_calculate(pidtype *p);                                          /* 计算一次输出 */

#endif
