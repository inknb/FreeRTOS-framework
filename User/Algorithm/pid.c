#include "pid.h"

// 限制函数：防止数值超出范围
static void limit(float *val, float max) {
    if (*val > max) *val = max;
    if (*val < -max) *val = -max;
}

// PID 初始化
void PID_Init(pid_t *pid, float kp, float ki, float kd, float max_out, float max_iout) {
    pid->kp = kp;
    pid->ki = ki;
    pid->kd = kd;
    pid->max_out = max_out;
    pid->max_iout = max_iout;
    // 清空历史数据
    pid->err[0] = pid->err[1] = pid->err[2] = 0.0f;
    pid->out = pid->iout = 0.0f;
}

// PID 计算核心 (位置式 PID)
// get: 实际测量值 (比如电机反馈转速)
// set: 期望目标值 (比如我想让他转 1000rpm)
void PID_Calc(pid_t *pid, float get, float set) {
    pid->fdb = get;
    pid->set = set;
    pid->err[0] = set - get; // 计算当前误差

    // 1. 计算 P 项 (比例)
    pid->pout = pid->kp * pid->err[0];

    // 2. 计算 I 项 (积分)
    pid->iout += pid->ki * pid->err[0];
    // --- 关键点：积分限幅 ---
    // 如果不加这个，误差一直存在时，积分项会无穷大，导致电机失控
    limit(&pid->iout, pid->max_iout);

    // 3. 计算 D 项 (微分)
    pid->dout = pid->kd * (pid->err[0] - pid->err[1]);

    // 4. 计算总输出
    pid->out = pid->pout + pid->iout + pid->dout;
    // --- 关键点：总输出限幅 ---
    // 限制发送给电机的电流值，保护电机
    limit(&pid->out, pid->max_out);

    // 5. 更新误差历史
    pid->err[2] = pid->err[1];
    pid->err[1] = pid->err[0];
}