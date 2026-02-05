#ifndef __BSP_RC_H
#define __BSP_RC_H

#include <stdint.h>

// 遥控器数据结构体
typedef struct {
    /* 摇杆通道 (数值范围: -660 ~ 660) */
    int16_t ch[5]; // ch[0]:右横, ch[1]:右纵, ch[2]:左横, ch[3]:左纵, ch[4]:拨轮

    /* 拨杆开关 (1:上, 3:中, 2:下) */
    uint8_t s[2];  // s[0]:右开关, s[1]:左开关

} rc_info_t;

// 声明全局变量，给 APP 层调用
extern rc_info_t rc;

// 初始化函数
void RC_Init(void);

#endif