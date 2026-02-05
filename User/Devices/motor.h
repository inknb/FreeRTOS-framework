#ifndef __MOTOR_H
#define __MOTOR_H

#include <stdint.h>
#include "pid.h"

// 定义电机数据结构体
typedef struct {
    // --- 原始反馈数据 (从 CAN 包里解出来的) ---
    uint16_t ecd;          // 转子机械角度 (0 ~ 8191)
    int16_t  speed_rpm;    // 转速 (rpm)
    int16_t  given_current;// 实际转矩电流
    uint8_t  temperate;    // 温度

    // --- 处理后的物理数据 (给控制闭环用的) ---
    int16_t  last_ecd;     // 上一次的机械角度 (用于计算圈数)
    float    total_angle;  // 总角度 (累积多圈的角度，比如转了10圈就是 3600度)
    int32_t  total_round;  // 总圈数
    
    // --- 控制相关 ---
    // 这里留个位置，后面第五阶段我们会把 PID 结构体放进来
   // --- 新增：每个电机自带两个 PID 控制器 ---
    pid_t pid_speed; // 速度环 PID (控制转速)
    pid_t pid_angle; // 角度环 PID
    
} motor_measure_t;

// 函数声明：解码函数
void motor_decode(motor_measure_t *motor, uint8_t *rx_data);

#endif