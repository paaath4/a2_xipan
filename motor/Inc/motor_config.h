#ifndef _MOTOR_CONFIG_H_
#define _MOTOR_CONFIG_H_

/* ============ 电机配置宏 ============ */
#define MOTOR_NUM       1U        /* 电机数量 */
#define M3508_RATIO     19.2f     /* 3508 减速比 3591/187 */
#define M3508_PULSE     8191U     /* 编码器每圈脉冲(反馈角 0~8191) */
#define M3508_CUR_LIM   12000     /* 电流限幅 raw(≈15A, D620 满值 20A=16384) */

#endif
