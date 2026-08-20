# 河海大学智泽实验室 2026 招新题目完成说明

姓名：崔莹雪  
学号：2523040306

## 目录说明

本目录为招新题目提交材料，包含基础题、软件类题目、硬件类题目和自我介绍。

| 目录 | 内容 |
| --- | --- |
| `Base_A` | Markdown 与基础 HTML5 学习文档 |
| `Base_B` | Git 基础操作、分支协作、冲突处理、`.gitignore` 与 README 学习文档 |
| `Base_C` | Linux 环境、Shell 指令、文件查找、文本处理、磁盘查看与压缩打包学习文档 |
| `Base_D` | 科学上网、开发账户、网络代理与账号安全学习文档 |
| `Software_A1` | C++ 有理数类设计工程、源码与学习文档 |
| `Software_A2` | C++ 图形类体系设计工程、源码与学习文档 |
| `Software_D1` | 强化学习基础概念学习文档 |
| `Software_D2` | 井字棋强化学习智能体源码、训练结果与学习文档 |
| `Software_E1` | 手写 BP/MLP 神经网络源码与学习文档 |
| `Hardware_A1` | 仪表放大器 Multisim 仿真、截图与学习文档 |
| `Hardware_A2` | 仪表放大器放大倍数推导与应用说明 |
| `Hardware_C1` | 51 单片机 PWM 驱动蜂鸣器与直流电机工程、实操记录与学习文档 |
| `Hardware_C2` | 51 单片机 OLED 滚动显示工程、实操记录与学习文档 |
| `Hardware_C3` | 51 单片机键盘拨号与 PC 通信工程、实操记录与学习文档 |
| `Hardware_D1` | STM32 PWM 驱动直流编码电机工程、实操记录与学习文档 |
| `Hardware_D2` | STM32 OLED 显示与单环 PID 控制工程、实操记录与学习文档 |
| `Hardware_D3` | STM32 串级 PID 与控制算法设计工程、实操记录与学习文档 |

## 软件题运行说明

`Software_A1`、`Software_A2` 为 Visual Studio C++ 工程，可通过对应 `.sln` 文件打开、编译和运行。源码位于各自工程子目录中的 `main.cpp`。

如果使用 VSCode 或命令行，也可以用 g++ 编译运行：

```bash
g++ -std=c++17 -o Software_A1.exe Software_A1/Software_A1/main.cpp
g++ -std=c++17 -o Software_A2.exe Software_A2/Software_A2/main.cpp
```

`Software_D2` 使用 Python 实现井字棋强化学习训练，进入目录后运行：

```bash
python Software_D2.py
```

运行后会输出训练结果，并在 `results` 目录中保存训练曲线和值函数文件。

`Software_E1` 使用 Python 和 NumPy 手写多层感知机，进入目录后运行：

```bash
python Software_E1.py
```

程序会训练网络拟合一元函数和二元函数，并输出误差结果。

## 硬件题说明

硬件题目录中保留了工程源码、学习文档和实操记录。实操记录包括编译日志、接线说明、实物图、演示视频和调试截图等材料。Keil 或 Multisim 工程可通过对应工程文件打开查看。

## 自我介绍

自我介绍文件位于根目录，文件名为 `自我介绍.pdf`。
