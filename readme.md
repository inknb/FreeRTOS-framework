# RoboMaster 步兵机器人电控框架

![Build Status](https://img.shields.io/badge/Build-Keil_MDK-brightgreen)
![Platform](https://img.shields.io/badge/Platform-STM32F4-blue)
![License](https://img.shields.io/badge/License-AGPL-orange)


## 📖 项目简介

本项目为 RoboMaster 步兵机器人的嵌入式底层控制代码。基于 **STM32F427** 平台，采用 **FreeRTOS** 架构。



---



\## 🏗️ 软件架构



项目代码位于 `User/` 目录，严格遵循分层解耦原则：



```

User/

├── APP/                    # 应用任务层
│   ├── chassis\_task.c     # 底盘运动控制（麦轮解算 + 速度环 PID）
│   └── gimbal\_task.c      # 云台稳像控制（串级 PID + IMU 反馈）
├── Algorithm/              # 算法层
│   └── pid.c              # 通用 PID 控制器实现
├── Devices/                # 设备抽象层
│   └── motor.c            # 大疆电机协议解析（M3508 / GM6020）
└── BSP/                    # 板级支持包
      ├── bsp\_can.c          # CAN 总线收发（电机通信）
      ├── bsp\_rc.c             # 遥控器 DBUS 解析（DR16）
      └── bsp\_imu.c          # 姿态传感器驱动（HWT606）

```



---



\## 🎮 主要功能



\### 底盘控制

\- 支持麦克纳姆轮\*\*逆运动学解算\*\*，实现前后、左右平移及自旋的全向移动

\- 电机闭环：单级 PID 速度环控制

\- 遥控器左摇杆控制平移，右摇杆控制旋转



\### 云台控制

\- 支持 \*\*IMU 闭环稳像\*\*，底盘运动时云台锁定世界坐标系

\- 控制算法：串级 PID（外环角度 + 内环速度）

\- 集成维特智能 HWT606 姿态传感器

\- 遥控器右摇杆控制云台俯仰/偏航



---



\## 🛠️ 硬件环境



| 组件 | 型号 | 通信接口 | 备注 |

|------|------|----------|------|

| 主控芯片 | STM32F427IIH6 | - | RoboMaster C 型开发板 |

| 底盘电机 | DJI M3508 P19 + C620 电调 | CAN1 (ID: 0x201-0x204) | 波特率 1Mbps |

| 云台电机 | DJI GM6020 | CAN2 (ID: 0x205-0x206) | 控制 ID: 0x1FF |

| 遥控接收 | DJI DR16 | USART3 (DBUS) | 100k, 9Bit, Even, DMA |

| IMU | 维特智能 HWT606 | USART6 (TTL) | 9600, 8Bit, None, DMA |

| 系统时基 | TIM1 | - | FreeRTOS Tick (1000Hz) |



---





\## 📋 开发计划



\- \[x] 基础控制框架搭建 (MVP)

\- \[x] 遥控器与底盘闭环

\- \[x] 云台 IMU 稳像

\- \[ ] 接入裁判系统串口解算

\- \[ ] 增加发射机构控制（摩擦轮 + 拨弹轮）

\- \[ ] 功率控制算法（超级电容）

\- \[ ] 适配 MiniPC 自瞄数据接口



---



\## 📄 开源协议

\[此处填写开源协议，例如 MIT License]



---



\## 📬 联系与贡献

如有问题或建议，欢迎提交 Issue 或 Pull Request！



---

> 本框架旨在为 RoboMaster 参赛队伍提供一个可快速上手、易于扩展的电控基础，欢迎 Star ⭐ 和 Fork！

