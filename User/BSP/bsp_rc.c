#include "bsp_rc.h"
#include "bsp_imu.h"
#include "usart.h" // 引用 CubeMX 生成的串口头文件

// 遥控器实例
rc_info_t rc;

// 接收缓冲区 (DBUS 一帧是 18 字节，我们开大点防止溢出)
uint8_t rc_buf[36]; 

// 初始化：开启 DMA 接收
void RC_Init(void) {
    // 开启串口3 DMA 接收，数据存入 rc_buf，长度 36
    // 注意：这里使用的是二倍缓冲区的技巧，或者直接接18字节也可以
    // 为了简单，我们先按标准 18 字节 + 循环接收来写
    HAL_UART_Receive_DMA(&huart3, rc_buf, 18);
}
// 外部引用的 IMU 缓冲区 (在 bsp_imu.c 里定义的)
extern uint8_t imu_rx_buf[];
// 串口中断回调函数 (CubeMX 的 HAL 库会自动调用这个)
// 也就是每次 DMA 收满 18 字节，就会进这个函数
// --- 统一的串口回调函数 ---
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    
    // --- 情况 1: 遥控器 (USART3) ---
    if(huart->Instance == USART3) {
        // (把之前的解码逻辑放这里)
        rc.ch[0] = ((rc_buf[0] | rc_buf[1] << 8) & 0x07FF) - 1024;
        // ... (省略中间代码) ...
        rc.s[1] = ((rc_buf[5] >> 4) & 0x0003);
    }
    
    // --- 情况 2: HWT606 姿态传感器 (USART6) ---
    else if(huart->Instance == USART6) {
        // 调用我们刚才写的解码函数
        IMU_Decode_Callback(imu_rx_buf);
        
        // 注意：如果是 Circular 模式，不需要重新开启 DMA
        // 但如果遇到 DMA 停止的情况，可以在这里加一句 HAL_UART_Receive_DMA
    }
}