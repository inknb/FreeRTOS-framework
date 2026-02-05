\# RoboMaster Infantry Control Framework (MVP)



!\[Language](https://img.shields.io/badge/Language-C-blue.svg)

!\[Platform](https://img.shields.io/badge/Platform-STM32-green.svg)

!\[RTOS](https://img.shields.io/badge/RTOS-FreeRTOS-orange.svg)



\## 📖 项目简介



本项目是一个基于 \*\*STM32F4\*\* 和 \*\*FreeRTOS\*\* 的 RoboMaster 步兵机器人电控基础框架。

项目采用 \*\*分层架构 (BSP / Device / Algorithm / APP)\*\* 设计，实现了底盘全向移动、云台 IMU 稳像控制以及遥控器人机交互。



该框架旨在提供一个模块化、易维护的“最小可行性产品 (MVP)”，适合作为新车开发的基石或电控队员的学习模板。



\## 🏗️ 软件架构



项目代码位于 `User/` 目录下，严格遵循分层解耦原则：



```text

User/

├── APP/            # 任务层 (Tasks)

│   ├── chassis\_task.c    # 底盘运动控制 (麦轮解算 + 速度环PID)

│   └── gimbal\_task.c     # 云台稳像控制 (串级PID + IMU反馈)

├── Algorithm/      # 算法层 (纯数学逻辑)

│   └── pid.c             # 通用 PID 控制器实现

├── Devices/        # 设备层 (硬件抽象)

│   └── motor.c           # 大疆电机协议解析 (M3508/GM6020)

└── BSP/            # 驱动层 (板级支持包)

&nbsp;   ├── bsp\_can.c         # CAN 总线收发 (电机通信)

&nbsp;   ├── bsp\_rc.c          # 遥控器 DBUS 协议解析 (DR16)

&nbsp;   └── bsp\_imu.c         # 姿态传感器驱动 (HWT606 UART协议)

🎮 主要功能底盘控制 (Chassis)支持麦克纳姆轮 (Mecanum Wheel) 逆运动学解算。实现前后、左右平移及自旋的全向移动。电机闭环：单级 PID 速度环控制。云台控制 (Gimbal)支持 IMU 闭环稳像：无论底盘如何运动，云台始终锁定世界坐标系目标。控制算法：串级 PID (外环角度环 + 内环速度环)。集成维特智能 HWT606 姿态传感器。人机交互适配大疆 DR16 接收机。左摇杆控制底盘平移，右摇杆控制底盘旋转及云台俯仰/偏航。🛠️ 硬件环境主控芯片: STM32F427IIH6 (适配 RoboMaster C型开发板)底盘电机: DJI M3508 P19 + C620电调 (CAN1, ID: 0x201-0x204)云台电机: DJI GM6020 (CAN2, ID: 0x205-0x206 / Ctrl ID: 0x1FF)遥控接收: DJI DR16 (SBUS/DBUS 协议)IMU: 维特智能 HWT606 (TTL串口协议)⚙️ 引脚与外设配置外设引脚/参数用途备注CAN1PD0 / PD1底盘电机通信波特率 1MbpsCAN2PB12 / PB13云台电机通信波特率 1MbpsUSART3PB10 / PB11DR16 接收机100k, 9Bit, Even, DMAUSART6PG14 / PG9HWT606 IMU9600, 8Bit, None, DMAFreeRTOSTIM1 (Timebase)操作系统1000Hz Tick🚀 快速开始克隆仓库:Bashgit clone <repository\_url>

环境准备:安装 VS Code 及 Embedded IDE (EIDE) 插件。安装 ARMCC (AC5) 或 ARMCLANG (AC6) 编译器。安装 STM32CubeMX (用于底层配置修改)。编译与烧录:在 EIDE 中打开项目工作区。点击 Build 按钮编译 (确保无 Error)。连接 J-Link 或 ST-Link，点击 Flash 烧录。📝 开发计划 (To-Do)\[x] 基础控制框架搭建 (MVP)\[x] 遥控器与底盘闭环\[x] 云台 IMU 稳像\[ ] 接入裁判系统串口解算\[ ] 增加发射机构控制 (摩擦轮 + 拨弹轮)\[ ] 功率控制算法 (超级电容)\[ ] 适配 MiniPC 自瞄数据接口

