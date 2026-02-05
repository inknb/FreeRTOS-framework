#include "chassis_task.h"
#include "bsp_can.h"
#include "bsp_rc.h"   // 引入遥控器
#include "pid.h"
#include "cmsis_os.h"

// 定义底盘速度结构体
typedef struct {
    float vx; // 前后速度 (m/s 或 原始数值)
    float vy; // 左右平移速度
    float wz; // 旋转速度
} chassis_speed_t;

void Chassis_Task(void const * argument)
{
    // 1. PID 初始化 (参数需根据实际情况调)
    for(int i=0; i<4; i++) {
        PID_Init(&motor_chassis[i].pid_speed, 5.0f, 0.1f, 0.0f, 16000.0f, 2000.0f);
    }

    chassis_speed_t speed;
    int16_t wheel_target[4]; // 4个轮子的目标转速

    for(;;)
    {
        // --- 2. 获取遥控器输入 (映射到速度) ---
        // 遥控器范围是 -660 ~ +660
        // 我们设置一个简单的死区，防止摇杆没回中时机器人乱动
        if(rc.ch[3] > -10 && rc.ch[3] < 10) speed.vx = 0;
        else speed.vx = rc.ch[3]; // 左摇杆竖直：控制前后

        if(rc.ch[2] > -10 && rc.ch[2] < 10) speed.vy = 0;
        else speed.vy = rc.ch[2]; // 左摇杆水平：控制左右平移

        if(rc.ch[0] > -10 && rc.ch[0] < 10) speed.wz = 0;
        else speed.wz = rc.ch[0]; // 右摇杆水平：控制旋转

        // --- 3. 麦克纳姆轮逆运动学解算 (核心公式) ---
        // 不同的电机安装顺序，公式正负号可能不同。
        // 假设顺序：0:右前, 1:左前, 2:左后, 3:右后 (常见X型布局)
        // 这是一个经验公式，如果实际跑起来方向不对，需要调整符号
        wheel_target[0] =  speed.vx - speed.vy + speed.wz; // 右前
        wheel_target[1] =  speed.vx + speed.vy - speed.wz; // 左前
        wheel_target[2] =  speed.vx - speed.vy - speed.wz; // 左后
        wheel_target[3] =  speed.vx + speed.vy + speed.wz; // 右后

        // --- 4. PID 计算与执行 ---
        for(int i=0; i<4; i++) {
            // 计算 PID：输入当前测量转速，输入目标转速
            PID_Calc(&motor_chassis[i].pid_speed, motor_chassis[i].speed_rpm, wheel_target[i]);
        }

        // 发送电流
        CAN_Send_Motor_Current(
            (int16_t)motor_chassis[0].pid_speed.out,
            (int16_t)motor_chassis[1].pid_speed.out,
            (int16_t)motor_chassis[2].pid_speed.out,
            (int16_t)motor_chassis[3].pid_speed.out
        );

        osDelay(1);
    }
}