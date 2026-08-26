#include "pid.h"

/* 初始化 PID: 设定增益与方式, 清零累积 */
void pid_init(pidtype *p, float kp, float ki, float kd, pidmode_t mode)
{
    p->kp = kp;
    p->ki = ki;
    p->kd = kd;
    p->mode = mode;
    p->max_out = 10000.f;
    p->max_integral = 1000.f;
    p->enabled = 1;
    pid_reset(p);
}

/* 清零累积量 */
void pid_reset(pidtype *p)
{
    p->setval = p->curval = p->integral = p->last_e1 = p->last_e2 = 0.f;
}

/* 数值限幅 */
static float clampf(float x, float lo, float hi) { return x < lo ? lo : (x > hi ? hi : x); }

/* 计算一次输出 */
float pid_calculate(pidtype *p)
{
    float e = p->setval - p->curval, out;
    if (!p->enabled) return 0.f;

    if (p->mode == PID_POSITION)
    {
        /* 位置式: P + I + D, 绝对量输出 */
        p->integral = clampf(p->integral + e, -p->max_integral, p->max_integral);
        out = p->kp * e + p->ki * p->integral + p->kd * (e - p->last_e1);
        p->last_e1 = e;
    }
    else
    {
        /* 增量式: Δ输出, 由调用方累加 */
        p->integral = clampf(p->integral + e, -p->max_integral, p->max_integral);
        out = p->kp * (e - p->last_e1) + p->ki * e +
              p->kd * (e - 2.f * p->last_e1 + p->last_e2);
        p->last_e2 = p->last_e1;
        p->last_e1 = e;
    }
    return clampf(out, -p->max_out, p->max_out);
}
