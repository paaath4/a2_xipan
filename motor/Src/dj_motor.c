#include "dj_motor.h"
#include "can.h"

/* 电机对象数组 */
DJMotor g_motor[MOTOR_NUM];

/* 带峰值限幅 */
#define CLAMPPEAK(x, pk) (((x) > (pk)) ? (pk) : ((x) < -(pk)) ? -(pk) : (x))

/* 初始化 上电默认失能 */
void DJ_Init(void)
{
    for (uint32_t i = 0; i < MOTOR_NUM; i++)
    {
        DJMotor *m = &g_motor[i];
        m->id = (uint8_t)(i + 1U);
        m->begin = false;
        m->mode_set = DJ_Disable;
        m->mode_cur = DJ_Disable;

        m->param.gear_ratio      = 1.f;
        m->param.reduction_ratio = M3508_RATIO;
        m->param.pulse_per_round = M3508_PULSE;
        m->param.current_limit   = M3508_CUR_LIM;

        m->limit.pos_rpm_limit   = true;
        m->limit.speed_rpm_limit = 10000;
        m->limit.max_angle_deg   = 270.f;
        m->limit.min_angle_deg   = -270.f;

        m->status.is_setzero = true;

        pid_init(&m->pos_pid, 0.07f, 0.005f, 0.f, PID_POSITION);
        pid_init(&m->vel_pid, 5.5f, 0.3f, 0.01f, PID_INCREMENT);
    }
}

/* CAN2 初始化 滤波+开接收中断 */
void DJ_CAN_Init(void)
{
    CAN_FilterTypeDef f = {0};
    f.FilterBank = 14;                    /* CAN2 使用 filter bank 14 */
    f.FilterMode = CAN_FILTERMODE_IDMASK;
    f.FilterScale = CAN_FILTERSCALE_32BIT;
    f.FilterIdHigh = 0;
    f.FilterIdLow = 0;
    f.FilterMaskIdHigh = 0;
    f.FilterMaskIdLow = 0;
    f.FilterFIFOAssignment = CAN_RX_FIFO0;
    f.FilterActivation = ENABLE;
    f.SlaveStartFilterBank = 14;
    HAL_CAN_ConfigFilter(&hcan2, &f);
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

/* 计算转子里程 */
static void angle_calc(DJMotor *m)
{
    m->val_now.pulse_gap = (int16_t)(m->val_now.pulse_read - m->val_pre.pulse_read);
    if (m->val_now.pulse_gap > (int16_t)(m->param.pulse_per_round / 2))
        m->val_now.pulse_gap = (int16_t)(m->val_now.pulse_gap - ((int32_t)m->param.pulse_per_round + 1));
    else if (m->val_now.pulse_gap < -(int16_t)(m->param.pulse_per_round / 2))
        m->val_now.pulse_gap = (int16_t)(m->val_now.pulse_gap + ((int32_t)m->param.pulse_per_round + 1));

    m->val_now.pulse_total += m->val_now.pulse_gap;
    m->val_now.angle_deg = (float)m->val_now.pulse_total * 360.f /
            ((float)m->param.pulse_per_round * m->param.gear_ratio * m->param.reduction_ratio);

    if (m->status.is_setzero)
    {
        m->val_now.pulse_total = 0;
        m->val_now.angle_deg = 0.f;
        m->status.is_setzero = false;
    }
    m->val_pre = m->val_now;
}

/* 接收解析 */
void DJ_Receive(CAN_RxHeaderTypeDef h, uint8_t *d)
{
    if (h.StdId < 0x201U || h.StdId > 0x208U) return;
    uint8_t id = (uint8_t)(h.StdId - 0x200U);
    if (id > MOTOR_NUM) return;

    DJMotor *m = &g_motor[id - 1U];
    m->val_now.pulse_read  = (int16_t)(((uint16_t)d[0] << 8) | d[1]);
    m->val_now.speed_rpm   = (int16_t)(((uint16_t)d[2] << 8) | d[3]);
    m->val_now.current_raw = (int16_t)(((uint16_t)d[4] << 8) | d[5]);
    m->val_now.temperature = (int8_t)d[6];
    m->val_now.current_A   = (float)m->val_now.current_raw / 1000.f;
    m->val_now.speed_rpm   = (int16_t)((float)m->val_now.speed_rpm /
            (m->param.gear_ratio * m->param.reduction_ratio));
    m->error.last_rx_tick = 0;
    angle_calc(m);
}

/* 电流发送 */
static void transmit(DJMotor *m)
{
    CAN_TxHeaderTypeDef th = {0};
    uint8_t d[8] = {0};
    th.IDE = CAN_ID_STD;
    th.RTR = CAN_RTR_DATA;
    th.DLC = 8;
    if (m->id <= 4U)
    {
        th.StdId = 0x200U;
        d[(m->id - 1U) * 2U]      = (uint8_t)(m->val_set.current_raw >> 8);
        d[(m->id - 1U) * 2U + 1U] = (uint8_t)m->val_set.current_raw;
    }
    else
    {
        th.StdId = 0x1FFU;
        d[(m->id - 5U) * 2U]      = (uint8_t)(m->val_set.current_raw >> 8);
        d[(m->id - 5U) * 2U + 1U] = (uint8_t)m->val_set.current_raw;
    }
    HAL_CAN_AddTxMessage(&hcan2, &th, d, NULL);
}

/* 模式切换 */
static void switch_mode(DJMotor *m)
{
    if (m->mode_set != m->mode_cur)
    {
        m->mode_cur = m->mode_set;
        m->val_set.current_raw = 0;
        m->val_set.speed_rpm = 0;
        m->val_set.angle_deg = m->val_now.angle_deg;
        pid_reset(&m->pos_pid);
        pid_reset(&m->vel_pid);
    }
}

/* 位置模式 */
static void pos_mode(DJMotor *m)
{
    m->pos_pid.setval = m->val_set.angle_deg * m->param.gear_ratio *
            m->param.reduction_ratio * (float)m->param.pulse_per_round / 360.f;
    m->pos_pid.curval = (float)m->val_now.pulse_total;
    m->vel_pid.setval = pid_calculate(&m->pos_pid);
    m->vel_pid.curval = (float)m->val_now.speed_rpm * m->param.gear_ratio *
            m->param.reduction_ratio;
    m->val_set.current_raw += (int16_t)pid_calculate(&m->vel_pid);
    m->val_set.current_raw = (int16_t)CLAMPPEAK(m->val_set.current_raw, m->param.current_limit);
}

/* 速度模式 */
static void rpm_mode(DJMotor *m)
{
    m->vel_pid.setval = (float)m->val_set.speed_rpm * m->param.gear_ratio *
            m->param.reduction_ratio;
    m->vel_pid.curval = (float)m->val_now.speed_rpm * m->param.gear_ratio *
            m->param.reduction_ratio;
    m->val_set.current_raw += (int16_t)pid_calculate(&m->vel_pid);
    m->val_set.current_raw = (int16_t)CLAMPPEAK(m->val_set.current_raw, m->param.current_limit);
}

/* 状态机 */
void DJ_Func(void)
{
    for (uint32_t i = 0; i < MOTOR_NUM; i++)
    {
        DJMotor *m = &g_motor[i];
        m->error.last_rx_tick++;
        if (m->error.last_rx_tick > 200)
        {
            m->status.overtime_flag = true;
            m->mode_set = DJ_Disable;
        }
        switch_mode(m);

        if (!m->begin)
        {
            m->val_set.current_raw = 0;
        }
        else
        {
            switch (m->mode_cur)
            {
            case DJ_Disable:  m->val_set.current_raw = 0; break;
            case DJ_Current:  m->val_set.current_raw = (int16_t)CLAMPPEAK(m->val_set.current_raw, m->param.current_limit); break;
            case DJ_RPM:      rpm_mode(m); break;
            case DJ_Position: pos_mode(m); break;
            default:          m->val_set.current_raw = 0; break;
            }
        }
        transmit(m);
    }
}

/* 应用层控制接口 */
void DJ_SetMode(uint8_t idx, dj_mode_t mode)
{
    if (idx && idx <= MOTOR_NUM) g_motor[idx - 1U].mode_set = mode;
}
void DJ_SetPos(uint8_t idx, float deg)
{
    if (idx && idx <= MOTOR_NUM) g_motor[idx - 1U].val_set.angle_deg = deg;
}
void DJ_SetSpeed(uint8_t idx, int16_t rpm)
{
    if (idx && idx <= MOTOR_NUM) g_motor[idx - 1U].val_set.speed_rpm = rpm;
}
void DJ_SetCurrent(uint8_t idx, int16_t cur)
{
    if (idx && idx <= MOTOR_NUM) g_motor[idx - 1U].val_set.current_raw = cur;
}
float DJ_GetPos(uint8_t idx)
{
    return (idx && idx <= MOTOR_NUM) ? g_motor[idx - 1U].val_now.angle_deg : 0.f;
}
