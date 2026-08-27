#include "CAN_IRQHandler.h"
#include "DJmotor.h"
#include "xipan_ctrl.h"

/* CAN FIFO0 接收: CAN1=主控命令(扩展帧), CAN2=电机反馈(标准帧) */
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan)
{
    CAN_RxHeaderTypeDef rx;
    uint8_t d[8];

    if (hcan->Instance == CAN1) {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx, d) == HAL_OK) {
            Xipan_OnCan(&rx, d);
        }
    } else if (hcan->Instance == CAN2) {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx, d) == HAL_OK) {
            DJmotor_Receive(rx, d);
        }
    }
}
