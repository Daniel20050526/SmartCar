/**
 * @file    spi.h
 * @brief   ST7735S LCD 的 SPI 通信引脚定义与控制宏
 * @note    引脚分配（SPI2 硬件外设 + GPIO 软件控制）：
 *          - SCK  时钟      : PB13（SPI2_SCK）
 *          - MOSI 主机输出  : PB15（SPI2_MOSI）
 *          - CS   片选      : PA5 （软件控制，低有效）
 *          - RES  复位      : PB12（软件控制，低有效）
 *          - DC   数据/命令 : PA12（软件控制，0=命令 1=数据）
 *          - BL   背光      : PA4 （软件控制，1=点亮）
 *          数据线只用 MOSI（单线写），SPI 时钟由 APB1 提供
 */
#ifndef __SPI_H
#define __SPI_H			  	 
#include "delay.h"
#include "stm32f10x_spi.h"



#define LCD_SPI                       SPI2
#define LCD_SPI_CLK                   RCC_APB1Periph_SPI2
#define LCD_SPI_CLK_CMD							 	RCC_APB1PeriphClockCmd

#define LCD_SPI_SCK_PIN               GPIO_Pin_13                 
#define LCD_SPI_SCK_GPIO_PORT         GPIOB                      
#define LCD_SPI_SCK_GPIO_CLK          RCC_APB2Periph_GPIOB

#define LCD_SPI_MOSI_PIN              GPIO_Pin_15                 
#define LCD_SPI_MOSI_GPIO_PORT        GPIOB                   
#define LCD_SPI_MOSI_GPIO_CLK         RCC_APB2Periph_GPIOB

#define LCD_CS_PIN                    GPIO_Pin_5              
#define LCD_CS_GPIO_PORT              GPIOA                   
#define LCD_CS_GPIO_CLK               RCC_APB2Periph_GPIOA


#define LCD_RES_PIN                   GPIO_Pin_12              
#define LCD_RES_GPIO_PORT             GPIOB                  
#define LCD_RES_GPIO_CLK              RCC_APB2Periph_GPIOB

#define LCD_DC_PIN                    GPIO_Pin_12            
#define LCD_DC_GPIO_PORT              GPIOA                  
#define LCD_DC_GPIO_CLK               RCC_APB2Periph_GPIOA

#define LCD_BL_PIN                    GPIO_Pin_4             
#define LCD_BL_GPIO_PORT              GPIOA                  
#define LCD_BL_GPIO_CLK               RCC_APB2Periph_GPIOA





//片选控制
#define LCD_CS_CMD(x)		(x)?(GPIO_SetBits(LCD_CS_GPIO_PORT, LCD_CS_PIN)):(GPIO_ResetBits(LCD_CS_GPIO_PORT, LCD_CS_PIN))
//数据和命令引脚控制
#define LCD_DC_CMD(x)		(x)?(GPIO_SetBits(LCD_DC_GPIO_PORT, LCD_DC_PIN)):(GPIO_ResetBits(LCD_DC_GPIO_PORT, LCD_DC_PIN))
//复位引脚控制
#define LCD_RES_CMD(x)	(x)?(GPIO_SetBits(LCD_RES_GPIO_PORT, LCD_RES_PIN)):(GPIO_ResetBits(LCD_RES_GPIO_PORT, LCD_RES_PIN))
//背光控制
#define LCD_BL_CMD(x)	(x)?(GPIO_SetBits(LCD_BL_GPIO_PORT, LCD_BL_PIN)):(GPIO_ResetBits(LCD_BL_GPIO_PORT, LCD_BL_PIN))







void SPI2_Init(void);
void SPI2_SetSpeed(uint8_t SPI_BaudRatePrescaler);
uint8_t SPI2_ReadWriteByte(uint8_t TxData);




#endif




