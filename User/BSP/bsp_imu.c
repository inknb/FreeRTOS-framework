#include "bsp_imu.h"
#include "usart.h" 

imu_t imu;
uint8_t imu_rx_buf[44]; // 接收缓冲区

// 初始化：开启 DMA
void IMU_Init(void) {
    // 使用 USART6，接收长度 44 (容纳4个包，确保能抓到完整数据)
    HAL_UART_Receive_DMA(&huart6, imu_rx_buf, 44);
}

// 核心解码逻辑
// 这个函数不需要你在 main 里调，它会被串口中断自动调用
void IMU_Decode_Callback(uint8_t *data) {
    // 简单的协议解析：遍历缓冲区寻找 0x55 0x53 (角度包)
    // 维特协议：包头(0x55) + 类型(0x53) + ...

    // 这种遍历是简化版，严谨做法是环形队列，但对于现在的你够用了
    for(int i = 0; i < 44 - 11; i++) {
        if(data[i] == 0x55 && data[i+1] == 0x53) {

            // 找到角度包，开始解码
            // 偏移: roll(2,3), pitch(4,5), yaw(6,7)
            // 公式: (High<<8 | Low) / 32768.0 * 180.0

            imu.roll  = (short)(data[i+3]<<8 | data[i+2]) / 32768.0f * 180.0f;
            imu.pitch = (short)(data[i+5]<<8 | data[i+4]) / 32768.0f * 180.0f;
            imu.yaw   = (short)(data[i+7]<<8 | data[i+6]) / 32768.0f * 180.0f;

            // 找到一个包就够了，退出循环
            return;
        }
    }
}