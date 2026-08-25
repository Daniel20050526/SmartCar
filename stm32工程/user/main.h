/**
 * @file    main.h
 * @brief   主模块头文件：汇总工程依赖的驱动头文件，并声明系统级配置函数
 */
#ifndef __MAIN_H
#define __MAIN_H


#include "stm32f10x.h"
#include "delay.h"
#include "drive.h"


/**
 * @brief  关闭 JTAG、仅保留 SWD 调试接口
 * @note   STM32 复位后 PA15/PB3/PB4 默认被 JTAG 占用；
 *         禁用 JTAG 后这三个引脚可作为普通 GPIO 使用（本工程 PB3 用于 LCD 背光等）
 */
inline void JTAG_SW_Config(void)
{
	//关闭JTAG接口，开启SW接口--PA15、PB3、PB4可用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);      // 开启 AFIO 时钟（引脚重映射功能需要）
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  // 禁用 JTAG，仅保留 SWD
}		

#endif
