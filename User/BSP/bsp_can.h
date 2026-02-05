#ifndef __BSP_CAN_H
#define __BSP_CAN_H

#include "can.h" // 引用 CubeMX 生成的头文件
#include "motor.h"
extern motor_measure_t motor_chassis[4];//底盘
extern motor_measure_t motor_gimbal[2];//云台
// CAN 初始化函数：配置过滤器并启动 CAN
void CAN_Init(void);

// 发送电流值给底盘电机 (假设 ID 为 1-4)
// 这里的参数对应 4 个电机的电流值
void CAN_Send_Motor_Current(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4);
void CAN_Send_Gimbal_Voltage(int16_t yaw, int16_t pitch);
#endif