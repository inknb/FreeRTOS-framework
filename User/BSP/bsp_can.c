#include "bsp_can.h"
#include "motor.h" // 一定要包含这个，否则不认识 motor_decode

// 全局变量定义
motor_measure_t motor_chassis[4];
motor_measure_t motor_gimbal[2];  // 新增：云台电机 [0]Yaw, [1]Pitch

// CAN 初始化函数
void CAN_Init(void) {
    CAN_FilterTypeDef can_filter;

    // =========================
    //       CAN1 配置 (底盘)
    // =========================
    can_filter.FilterBank = 0;
    can_filter.FilterMode = CAN_FILTERMODE_IDMASK;
    can_filter.FilterScale = CAN_FILTERSCALE_32BIT;
    can_filter.FilterIdHigh = 0;
    can_filter.FilterIdLow = 0;
    can_filter.FilterMaskIdHigh = 0;
    can_filter.FilterMaskIdLow = 0;
    can_filter.FilterFIFOAssignment = CAN_RX_FIFO0;
    can_filter.FilterActivation = ENABLE;
    can_filter.SlaveStartFilterBank = 14;

    HAL_CAN_ConfigFilter(&hcan1, &can_filter);
    HAL_CAN_Start(&hcan1);
    HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING);

    // =========================
    //       CAN2 配置 (云台)
    // =========================
    // 注意：CAN2 的过滤器必须从 SlaveStartFilterBank (14) 开始
    can_filter.FilterBank = 14; 
    
    // 这里的配置和上面一样，复用结构体即可，只要改 FilterBank 和 hcan2
    HAL_CAN_ConfigFilter(&hcan2, &can_filter); 
    HAL_CAN_Start(&hcan2);
    HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING);
}

// 发送底盘电机电流 (0x200)
void CAN_Send_Motor_Current(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4) {
    CAN_TxHeaderTypeDef tx_header;
    uint8_t tx_data[8];
    uint32_t mailbox;

    tx_header.StdId = 0x200;
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;

    tx_data[0] = motor1 >> 8;
    tx_data[1] = motor1;
    tx_data[2] = motor2 >> 8;
    tx_data[3] = motor2;
    tx_data[4] = motor3 >> 8;
    tx_data[5] = motor3;
    tx_data[6] = motor4 >> 8;
    tx_data[7] = motor4;

    HAL_CAN_AddTxMessage(&hcan1, &tx_header, tx_data, &mailbox);
}

// 发送云台电机电压 (0x1FF)
void CAN_Send_Gimbal_Voltage(int16_t yaw, int16_t pitch) {
    CAN_TxHeaderTypeDef tx_header;
    uint8_t tx_data[8];
    uint32_t mailbox;

    tx_header.StdId = 0x1FF; // 云台控制 ID
    tx_header.IDE = CAN_ID_STD;
    tx_header.RTR = CAN_RTR_DATA;
    tx_header.DLC = 8;

    tx_data[0] = yaw >> 8;
    tx_data[1] = yaw;
    tx_data[2] = pitch >> 8;
    tx_data[3] = pitch;
    // 后4位留空
    tx_data[4] = 0;
    tx_data[5] = 0;
    tx_data[6] = 0;
    tx_data[7] = 0;

    HAL_CAN_AddTxMessage(&hcan2, &tx_header, tx_data, &mailbox);
}

// CAN 接收中断回调函数
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rx_header;
    uint8_t rx_data[8];

    HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_data);

    // --- 底盘数据 (CAN1) ---
    if (hcan->Instance == CAN1) {
        if (rx_header.StdId >= 0x201 && rx_header.StdId <= 0x204) {
            uint8_t index = rx_header.StdId - 0x201;
            motor_decode(&motor_chassis[index], rx_data);
        }
    }
    // --- 云台数据 (CAN2) ---
    else if (hcan->Instance == CAN2) {
        // 假设 ID1=Yaw, ID2=Pitch -> 反馈 0x205, 0x206
        if (rx_header.StdId >= 0x205 && rx_header.StdId <= 0x206) {
            uint8_t index = rx_header.StdId - 0x205; 
            motor_decode(&motor_gimbal[index], rx_data);
        }
    }
}