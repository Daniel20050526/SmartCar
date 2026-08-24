#include "drive.h"
#include "delay.h"

void LED_Config(void)
{
	//1.开时钟---APB2
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
	//2.结构体赋值
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;// 通用推挽输出
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;//翻转速率
	//3.初始化
	GPIO_Init(GPIOC,&GPIO_InitStruct);
	LED1_OFF;LED2_OFF;LED3_OFF;
}

void LED_Water(void)
{
	static int cnt=0;
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
	cnt%=3;
}

void BEEP_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_InitStruct);
	BEEP_OFF;
}

void KEY_Config(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	GPIO_InitTypeDef gpio_key = {0};
	gpio_key.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	gpio_key.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA,&gpio_key);
}

void Get_Key_Value(void)
{
	if(KEY1==0)
	{
		Delay_ms(10);//延时消抖
		if(KEY1==0)
		{
			BEEP_TOGGLE;
			while(KEY1==0);//松手检测
		}
	}
}

//长短按函数
uint8_t Key_Scan(void)
{
	static int count = 0;
	if(KEY1==0)
	{
		count++;//区分长短按
	}
	//按键松开判断count数值的大小-----按下时间的长短
	else{
		if(count>100)//长按
		{
			count = 0;
			return 1;
		}
		else if(count>2)//短按
		{
			count = 0;
			return 2;
		}
		else
		{
			count = 0;
		}
	}
	return 0;	
}




