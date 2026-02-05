#ifndef __BSP_IMU_H
#define __BSP_IMU_H

#include "struct_typedef.h" // 引用之前的类型定义

typedef struct {
    float roll;
    float pitch;
    float yaw;
} imu_t;

extern imu_t imu;

// 初始化函数
void IMU_Init(void);

// 供中断调用的解码函数
void IMU_Decode_Callback(uint8_t *data);

#endif