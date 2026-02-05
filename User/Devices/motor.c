#include "motor.h"

// 大疆电机协议解析函数
// 输入：电机结构体指针，CAN接收到的8字节数据
void motor_decode(motor_measure_t *motor, uint8_t *rx_data) {
    // 1. 保存上一次的角度，用来计算有没有跨过“0点”
    motor->last_ecd = motor->ecd;

    // 2. 解析原始数据 (大疆协议：高位在前)
    // 字节0-1: 机械角度
    motor->ecd = (rx_data[0] << 8) | rx_data[1];
    // 字节2-3: 转速
    motor->speed_rpm = (rx_data[2] << 8) | rx_data[3];
    // 字节4-5: 实际电流
    motor->given_current = (rx_data[4] << 8) | rx_data[5];
    // 字节6: 温度
    motor->temperate = rx_data[6];

    // 3. 多圈角度计算 (这是进阶操作，防止电机转过一圈后角度突变)
    // 如果刚刚从 8191 跳变到 0 (正转一圈)
    if (motor->ecd - motor->last_ecd < -4096) {
        motor->total_round++;
    }
    // 如果刚刚从 0 跳变到 8191 (反转一圈)
    else if (motor->ecd - motor->last_ecd > 4096) {
        motor->total_round--;
    }
    
    // 计算总角度 = 圈数 * 360 + 当前角度
    motor->total_angle = motor->total_round * 360.0f + (motor->ecd / 8191.0f * 360.0f);
}