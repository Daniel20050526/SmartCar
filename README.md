---
AIGC:
    Label: "1"
    ContentProducer: 001191440300708461136T1XGW3
    ProduceID: ab717a1d0f46268ae2740be0ca6c6f02_7587a8389fd411f1a238525400e6dd8f
    ReservedCode1: /oI9zkzFg1tu/Man5gHEoNKPIYzfiQrdgxTf+A/bPwKIzMZL1bhH2b259N2e25ywiXNkPDf86kEWug5Co8PScj6//isXxgRovwj8LcdJUMOo6g5cmIdIScbWotAUa8BHtpDt3RWZjn+2mUWpqnNhae4O7ZRHRztJT9rx1ZoIAkoJEIGcetNRJcgIc98=
    ContentPropagator: 001191440300708461136T1XGW3
    PropagateID: ab717a1d0f46268ae2740be0ca6c6f02_7587a8389fd411f1a238525400e6dd8f
    ReservedCode2: /oI9zkzFg1tu/Man5gHEoNKPIYzfiQrdgxTf+A/bPwKIzMZL1bhH2b259N2e25ywiXNkPDf86kEWug5Co8PScj6//isXxgRovwj8LcdJUMOo6g5cmIdIScbWotAUa8BHtpDt3RWZjn+2mUWpqnNhae4O7ZRHRztJT9rx1ZoIAkoJEIGcetNRJcgIc98=
---

# 智能车（STM32F103RC 外设驱动学习工程）

基于 STM32F103RC（Cortex-M3）的嵌入式学习项目。工程以一辆智能车为载体，逐步实现常见外设的驱动与控制：板载 LED 流水灯、蜂鸣器、按键（短按 / 长按识别），以及 1.8 寸 ST7735S TFT-LCD（SPI 接口）的显示驱动。代码基于 STM32 标准外设库（V3.5.0）编写，风格简洁，适合作为 STM32 入门与模块复用的参考。

## 功能特性

当前代码实现的功能（以仓库内实际源码为准）：

- **LED 流水灯**：3 路板载 LED（PC0 / PC1 / PC2）循环点亮，主程序以 500ms 间隔轮换，演示基本 GPIO 推挽输出。
- **蜂鸣器驱动**：PA15 输出控制，提供开 / 关 / 翻转宏，配合按键可作提示音。
- **按键检测**：PA0 按键输入，带 10ms 软件消抖、松手检测，并支持短按 / 长按区分（`Key_Scan`）。
- **SysTick 延时**：基于系统滴答定时器配置 + NOP 指令实现 `Delay_us` / `Delay_ms` 延时。
- **LCD 显示驱动（ST7735S）**：128×160 分辨率，硬件 SPI2 接口驱动；支持画点、画线、矩形、圆、字符 / 字符串、12/16/24/32 点阵汉字及图片显示，可横屏 / 竖屏切换。
- **工程化目录**：用户自写代码与 ST 官方标准库、资料分离，便于阅读与二次开发。

> 说明：本仓库为学习积累项目，不包含遥控、循迹、避障等未实现的整车功能。

## 目录结构

```
智能车/
├── stm32工程/                  # Keil MDK 工程
│   ├── project/                # 工程文件（Charging_Pile.uvprojx 等）
│   ├── startup/                # 启动文件 startup_stm32f10x_hd.s
│   ├── stdlib/                 # STM32 标准外设库（inc/src）
│   └── user/                   # 用户代码
│       ├── main.c / main.h     # 主程序（LED 流水灯）
│       ├── stm32f10x_it.c/h    # 中断服务函数模板
│       ├── system_stm32f10x.c  # 系统时钟配置文件
│       └── api/
│           ├── IO源码/         # drive.c/h：LED、蜂鸣器、按键驱动
│           └── delay/          # delay.c/h：SysTick 延时
├── 源码/
│   ├── LCD教程源码/            # ST7735S LCD 驱动（lcd.c/spi.c/lcdfont.h 等）
│   │   └── 2.1版本屏幕/LCD/    # 另一版本 LCD 驱动
│   └── STM32F10x_StdPeriph_Lib/  # ST 官方标准外设库 V3.5.0（完整版）
├── 资料/
│   ├── 官方资料/               # STM32 数据手册、参考手册、编程手册等
│   ├── 教程资料/               # ST7735S 数据手册、SPI 协议、LCD 显示原理等
│   └── 硬件图片/               # 硬件实物图
└── 软件/                       # 辅助工具素材（图片）
```

## 技术栈

| 项目 | 说明 |
| --- | --- |
| 主控芯片 | STM32F103RC（ARM Cortex-M3，72MHz） |
| 开发环境 | Keil MDK-ARM（工程为 `.uvprojx` 格式） |
| 固件库 | STM32F10x 标准外设库 V3.5.0（ST StdPeriph Lib） |
| LCD | ST7735S，128×160 TFT，硬件 SPI2 接口 |
| 调试下载 | ST-Link / J-Link（SWD） |

### LCD 引脚连接（参考 `spi.h`）

| 功能 | 引脚 |
| --- | --- |
| SPI2 SCK | PB13 |
| SPI2 MOSI | PB15 |
| 片选 CS | PA5 |
| 复位 RES | PB12 |
| 数据/命令 DC | PA12 |
| 背光 BL | PA4 |

## 使用说明

1. **打开工程**：使用 Keil MDK 打开 `stm32工程/project/Charging_Pile.uvprojx`（工程名为模板遗留名称，不影响编译）。
2. **编译**：点击 Build（F7），生成 Hex 文件（位于 `project/Objects/`）。
3. **烧录**：通过 ST-Link / J-Link 连接 SWD 接口，点击 Download（F8）下载程序。
4. **运行**：上电复位后，观察板载 3 路 LED 以 500ms 间隔循环流水点亮；按 KEY1 可触发蜂鸣器（需在代码中调用对应接口）。
5. **修改入口**：主程序位于 `stm32工程/user/main.c`，外设接口见 `drive.h` / `delay.h`；LCD 使用示例见 `源码/LCD教程源码/`。

## 开发环境要求

- Windows 操作系统
- Keil MDK-ARM 5.x（含 ARMCC 编译器）
- STM32F10x 标准外设库 V3.5.0（仓库已随附完整库，无需单独安装）
- ST-Link 或 J-Link 调试下载器
- 可选：USB-TTL 串口模块（用于后续串口调试扩展）

## 致谢与许可说明

- STM32 标准外设库、启动文件及部分底层模板代码版权归 STMicroelectronics 所有，遵循其原始许可条款。
- LCD 驱动源码参考开源 ST7735S 驱动实现，字库位于 `lcdfont.h`。
- 本仓库用于学习交流，请在遵守相关协议的前提下使用。
*（内容由AI生成，仅供参考）*
