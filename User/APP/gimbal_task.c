#include "gimbal_task.h"
#include "bsp_can.h"
#include "bsp_rc.h"
#include "pid.h"
#include "cmsis_os.h"
#include "bsp_imu.h" // 必须引用，我们要用 imu 变量

// 云台目标角度结构体
typedef struct {
    float yaw_angle;   // Yaw轴目标角度
    float pitch_angle; // Pitch轴目标角度
} gimbal_target_t;

gimbal_target_t gimbal_target;

// 云台初始化
void Gimbal_Init(void) {
    // 1. 初始化 Yaw 轴 PID
    // 角度环 (外环): 输入角度差(度)，输出目标速度(rpm)
    PID_Init(&motor_gimbal[0].pid_angle, 15.0f, 0.0f, 0.0f, 500.0f, 0.0f);
    // 速度环 (内环): 输入速度差(rpm)，输出电压/电流
    PID_Init(&motor_gimbal[0].pid_speed, 100.0f, 5.0f, 0.0f, 30000.0f, 5000.0f);

    // 2. 初始化 Pitch 轴 PID
    PID_Init(&motor_gimbal[1].pid_angle, 15.0f, 0.0f, 0.0f, 300.0f, 0.0f);
    PID_Init(&motor_gimbal[1].pid_speed, 100.0f, 5.0f, 0.0f, 30000.0f, 5000.0f);

    //  初始化目标角度为 IMU 当前角度 ---
    // 这样上电时，云台会“定”在当前朝向，而不是猛地转回电机零点
    // 注意：这里我们信任 HWT606 上电几秒后数据已经稳定
    gimbal_target.yaw_angle = imu.yaw;
    gimbal_target.pitch_angle = imu.pitch;
}

// 云台任务入口
void Gimbal_Task(void const * argument)
{
    // 任务开始前先初始化 PID 和 目标值
    Gimbal_Init();
    
    // 延时等待 IMU 数据完全稳定 (HWT606 上电需要几秒校准)
    osDelay(1000);

    for(;;)
    {
        // --- 1. 处理遥控器输入 ---
        // 灵敏度系数 0.005，意味着每 1ms 增加一点点角度
        
        // Yaw 轴控制 (左右)
        if(rc.ch[0] > 10 || rc.ch[0] < -10) {
            gimbal_target.yaw_angle -= rc.ch[0] * 0.005f; 
        }

        // Pitch 轴控制 (俯仰)
        if(rc.ch[1] > 10 || rc.ch[1] < -10) {
            gimbal_target.pitch_angle += rc.ch[1] * 0.005f;
        }

        // --- 2. 限制 Pitch 轴角度范围 ---
        // 防止云台撞到机械限位 (基于 IMU 角度限制)
        if(gimbal_target.pitch_angle > 25.0f) gimbal_target.pitch_angle = 25.0f;
        if(gimbal_target.pitch_angle < -25.0f) gimbal_target.pitch_angle = -25.0f;

        // --- 3. 串级 PID 计算 
        
        // === Yaw 轴 ===
        // 现在的逻辑是：目标是世界坐标系的度数，反馈也是世界坐标系的度数
        PID_Calc(&motor_gimbal[0].pid_angle, imu.yaw, gimbal_target.yaw_angle);
        
        // 内环不变：依然是控制电机的转速
        PID_Calc(&motor_gimbal[0].pid_speed, motor_gimbal[0].speed_rpm, motor_gimbal[0].pid_angle.out);


        // === Pitch 轴 ===
        // 反馈值从 motor.total_angle 改为 imu.pitch ---
        PID_Calc(&motor_gimbal[1].pid_angle, imu.pitch, gimbal_target.pitch_angle);
        

        PID_Calc(&motor_gimbal[1].pid_speed, motor_gimbal[1].speed_rpm, motor_gimbal[1].pid_angle.out);

        // --- 4. 发送 CAN 指令 ---
        CAN_Send_Gimbal_Voltage(
            (int16_t)motor_gimbal[0].pid_speed.out, // Yaw
            (int16_t)motor_gimbal[1].pid_speed.out  // Pitch
        );

        osDelay(1);
    }
}