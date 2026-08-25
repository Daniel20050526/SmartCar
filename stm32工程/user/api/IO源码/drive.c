/**
 * @file    drive.c
 * @brief   板载外设驱动：LED（PC0~PC2）、蜂鸣器（PA15）、按键（PA0）
 * @note    包含 GPIO 初始化、流水灯控制、按键消抖扫描与长短按识别
 */
#include "drive.h"
#include "delay.h"

/**
 * @brief  LED GPIO 初始化
 * @note   将 PC0/PC1/PC2 配置为推挽输出，初始全部熄灭
 */
void LED_Config(void)
{
	//1.开时钟---APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);   // 开启 GPIOC 外设时钟（GPIO 挂在 APB2 总线）
	//2.结构体赋值
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;// 通用推挽输出：可输出高低电平，驱动 LED
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//翻转速率：决定 IO 电平跳变速度
	//3.初始化
	GPIO_Init(GPIOC,&GPIO_InitStruct);   // 将结构体配置写入 GPIO 寄存器
	LED1_OFF;LED2_OFF;LED3_OFF;         // 初始熄灭（SetBits 输出高电平，LED 低电平点亮）
}

/**
 * @brief  流水灯状态机：依次点亮 LED1 -> LED2 -> LED3
 * @note   static 局部变量在函数返回后仍保留值，实现状态循环切换
 */
void LED_Water(void)
{
	static int cnt=0;    // static 变量：跨调用保存当前点亮到第几路
	switch(cnt)
	{
		case 0:LED1_ON;LED2_OFF;LED3_OFF;
			break;
		case 1:LED1_OFF;LED2_ON;LED3_OFF;
			break;
		case 2:LED1_OFF;LED2_OFF;LED3_ON;
			break;
	}
	cnt++;
	cnt%=3;              // 取模实现 0->1->2->0 循环
}

/**
 * @brief  蜂鸣器 GPIO 初始化（PA15 推挽输出）
 * @note   注意 PA15 默认被 JTAG 占用，使用前需调用 JTAG_SW_Config() 释放该引脚
 */
void BEEP_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   // 开启 GPIOA 时钟
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	BEEP_OFF;           // 初始关闭蜂鸣器
}

/**
 * @brief  按键 GPIO 初始化（PA0 浮空输入）
 * @note   按键一端接地，按下时 PA0 被拉低，读取到 0 表示按下
 */
void KEY_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef gpio_key = {0};
	gpio_key.GPIO_Mode = GPIO_Mode_IN_FLOATING;   // 浮空输入：引脚电平完全由外部电路决定
	gpio_key.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&gpio_key);
}

/**
 * @brief  按键处理：按下一次则翻转蜂鸣器开关（带消抖与松手检测）
 * @note   流程：检测按下 -> 延时 10ms 二次确认（消除机械抖动）-> 翻转蜂鸣器 -> 等待松手
 */
void Get_Key_Value(void)
{
	if(KEY1==0)               // 读到低电平：按键被按下
	{
		Delay_ms(10);         // 延时消抖：跳过按键按下瞬间的机械抖动
		if(KEY1==0)           // 二次确认仍为按下，判定为真实按键
		{
			BEEP_TOGGLE;      // 翻转蜂鸣器输出（响/不响切换）
			while(KEY1==0);   // 松手检测：等待按键释放，避免一次按下多次触发
		}
	}
}

/**
 * @brief  按键扫描：区分短按与长按
 * @retval 0-无动作  1-长按  2-短按
 * @note   原理：主循环周期性调用本函数，按下期间 count 持续累加；
 *         松开时依据 count 大小判断按下时长（count>100 判为长按）
 */
uint8_t Key_Scan(void)
{
	static int count = 0;
	if(KEY1==0)               // 按键保持按下：计数累加，用于区分长短按
	{
		count++;
	}
	//按键松开判断count数值的大小-----按下时间的长短
	else{
		if(count>100)         // 长按（count 累计超过阈值）
		{
			count = 0;
			return 1;
		}
		else if(count>2)      // 短按
		{
			count = 0;
			return 2;
		}
		else                  // 计数过小，视为噪声/误触，忽略
		{
			count = 0;
		}
	}
	return 0;	
}




