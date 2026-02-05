#ifndef __PID_H
#define __PID_H

#include <stdint.h>

// PID 参数结构体
typedef struct {
    // --- 参数区 (由用户初始化) ---
    float kp;
    float ki;
    float kd;
    float max_out;  // 最大输出限幅 (防止电机过热/过流)
    float max_iout; // 积分限幅 (防积分饱和，非常重要！)

    // --- 数据区 (计算过程中自动更新) ---
    float set;      // 目标值 (SetPoint)
    float fdb;      // 反馈值 (FeedBack)
    float err[3];   // 误差缓存: [0]当前, [1]上次, [2]上上次
    
    float pout;     // P项输出
    float iout;     // I项输出
    float dout;     // D项输出
    float out;      // 总输出
    
} pid_t;

// PID 计算函数声明
void PID_Calc(pid_t *pid, float get, float set);
// PID 初始化函数声明
void PID_Init(pid_t *pid, float kp, float ki, float kd, float max_out, float max_iout);

#endif