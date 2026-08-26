#ifndef _DJ_MOTOR_H_
#define _DJ_MOTOR_H_

#include "main.h"
#include "stdbool.h"
#include "pid.h"
#include "motor_config.h"

/* ============ 电机模式 ============ */
typedef enum
{
    DJ_Disable = 0,   /* 失能: 发 0 电流 */
    DJ_Current = 1,   /* 电流/力矩 */
    DJ_RPM = 2,       /* 速度环 */
    DJ_Position = 3   /* 位置环 */
} dj_mode_t;

/* ============ 反馈值 ============ */
typedef struct
{
    volatile int16_t current_raw;   /* 电流指令/反馈 raw */
    volatile int16_t speed_rpm;     /* 输出轴转速 rpm */
    volatile int16_t pulse_read;    /* 编码器原始脉冲 0~8191 */
    volatile int16_t pulse_gap;     /* 相邻两帧脉冲差 */
    volatile int32_t pulse_total;   /* 累计脉冲(位置里程) */
    volatile float   angle_deg;     /* 输出轴角度 ° */
    volatile float   current_A;     /* 反馈电流 A */
    volatile int8_t  temperature;   /* 温度 °C */
} dj_val_t;

/* ============ 电机参数 ============ */
typedef struct
{
    float    gear_ratio;        /* 机构传动比(直连 = 1.0) */
    float    reduction_ratio;   /* 电机减速比(3508 = 19.2) */
    uint16_t pulse_per_round;   /* 编码器每圈脉冲 */
    int16_t  current_limit;     /* 电流限幅 raw */
} dj_param_t;

/* ============ 限幅 ============ */
typedef struct
{
    bool    rpm_limit;           /* 速度模式限速使能 */
    bool    pos_angle_limit;     /* 位置角度限幅使能 */
    bool    pos_rpm_limit;       /* 位置环内速度限幅使能 */
    bool    loose_stuck;         /* 堵转后自动失能 */
    float   max_angle_deg;       /* 位置上限 ° */
    float   min_angle_deg;       /* 位置下限 ° */
    int16_t speed_rpm_limit;     /* 速度上限 rpm */
    int16_t zero_rpm_limit;      /* 寻零转速 rpm */
    int16_t zero_current_limit;  /* 寻零电流限幅 raw */
} dj_limit_t;

/* ============ 状态标志 ============ */
typedef struct
{
    bool is_setzero;      /* 请求当前位置清零 */
    bool overtime_flag;   /* 通信超时 */
    bool stuck_flag;      /* 堵转 */
    bool zero_flag;       /* 寻零完成 */
} dj_status_t;

/* ============ 监测计数 ============ */
typedef struct
{
    uint32_t zero_cnt;      /* 寻零停滞累计 */
    uint32_t stuck_cnt;     /* 堵转连续计数 */
    uint32_t timeout_cnt;   /* 超时连续计数 */
    uint16_t last_rx_tick;  /* 距上次收包 tick 数 */
} dj_error_t;

/* ============ 电机对象 ============ */
typedef struct
{
    uint8_t            id;        /* ID = 数组下标 + 1 */
    volatile bool      begin;     /* true: 使能运行; false: 发 0 电流 */
    volatile dj_mode_t mode_set;  /* 目标模式(应用层写) */
    volatile dj_mode_t mode_cur;  /* 实际模式(驱动层维护) */
    dj_param_t  param;
    dj_limit_t  limit;
    dj_status_t status;
    dj_error_t  error;
    dj_val_t    val_set;   /* 期望值, 应用层写 */
    dj_val_t    val_now;   /* 反馈值, 收包写 */
    dj_val_t    val_pre;   /* 上一帧反馈 */
    pidtype     pos_pid;   /* 位置环(位置式) */
    pidtype     vel_pid;   /* 速度环(增量式) */
} DJMotor, *DJMotorPtr;

extern DJMotor g_motor[MOTOR_NUM];

void DJ_Init(void);                                      /* 初始化参数/PID/状态 */
void DJ_CAN_Init(void);                                  /* CAN2 滤波+启动+开接收中断 */
void DJ_Func(void);                                      /* 1kHz 状态机 */
void DJ_Receive(CAN_RxHeaderTypeDef hdr, uint8_t *data); /* 收反馈帧 */

void DJ_SetMode(uint8_t idx, dj_mode_t mode);    /* 目标模式 */
void DJ_SetPos(uint8_t idx, float deg);          /* 位置模式目标角 */
void DJ_SetSpeed(uint8_t idx, int16_t rpm);      /* 速度模式目标 rpm */
void DJ_SetCurrent(uint8_t idx, int16_t cur);    /* 电流模式目标 raw */
float DJ_GetPos(uint8_t idx);                    /* 当前输出轴角度 */

#endif
