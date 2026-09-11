/*******************************************************************************
 * 模块名称：主模块头文件
 * 所属工程：智能车（STM32F103 + 板载 LED / 蜂鸣器 / 按键）
 * 模块职责：汇总工程依赖的驱动头文件，并声明系统级初始化函数 JTAG_SW_Config
 * 说明：本模块为自研代码，逻辑未改动，仅补充中文注释便于阅读维护
 ******************************************************************************/
#ifndef __MAIN_H
#define __MAIN_H


#include "stm32f10x.h"
#include "delay.h"
#include "drive.h"


/**
 * 函数名：JTAG_SW_Config
 * 描述  ：关闭 JTAG 调试接口、仅保留 SWD，释放 PA15/PB3/PB4 作为普通 IO
 * 输入  ：无
 * 输出  ：无
 * 说明  ：STM32 复位后 PA15/PB3/PB4 默认被 JTAG 占用；本工程 PA15 用于蜂鸣器
 *         控制、PB3 预留 LCD 背光，故须先关闭 JTAG 才能正常使用这些引脚
 */
inline void JTAG_SW_Config(void)
{
	//关闭JTAG接口，开启SW接口--PA15、PB3、PB4可用
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);      // 开启 AFIO 时钟（引脚重映射功能需要）
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);  // 禁用 JTAG，仅保留 SWD
}		

#endif
