/**
 * @file    lcd.c
 * @brief   ST7735S LCD 驱动：SPI 通信、初始化、绘图与字符/汉字显示
 * @note    显示原理：ST7735S 内置 GRAM（显存），通过 SPI2 写入命令/数据；
 *          RGB565 格式（16 位/像素），先设置访问窗口（坐标）再连续写入像素数据
 */
#include "lcd.h"
#include "lcdfont.h"
#include "delay.h"
#include "spi.h"


/******************************************************************************
      函数说明：LCD串行数据写入函数（底层 SPI 发送 1 字节）
      入口数据：dat  要写入的串行数据
      返回值：  无
      原理：拉低片选选中 LCD -> SPI2 移出一字节 -> 拉高片选释放总线
******************************************************************************/
void LCD_Writ_Bus(u8 dat) 
{	
	LCD_CS_CMD(0);          // 片选拉低：选中 LCD 从机
	SPI2_ReadWriteByte(dat);// SPI 全双工发送 1 字节（同步产生时钟）
	LCD_CS_CMD(1);          // 片选拉高：释放总线
}

/******************************************************************************
      函数说明：LCD写入数据（8 位）
      入口数据：dat 写入的数据
      返回值：  无
******************************************************************************/
void LCD_WR_DATA8(u8 dat)
{
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入数据（16 位，用于颜色值）
      入口数据：dat 写入的数据（RGB565 颜色）
      返回值：  无
      注意：先发高 8 位再发低 8 位，符合 ST7735S 大端字节序要求
******************************************************************************/
void LCD_WR_DATA(u16 dat)
{
	LCD_Writ_Bus(dat>>8);
	LCD_Writ_Bus(dat);
}


/******************************************************************************
      函数说明：LCD写入命令（寄存器指令）
      入口数据：dat 写入的命令
      返回值:无
      原理：DC 引脚区分命令/数据——DC=0 时写入的是命令，DC=1 时是数据
******************************************************************************/
void LCD_WR_REG(u8 dat)
{
	LCD_DC_CMD(0);//写命令（DC 拉低）
	LCD_Writ_Bus(dat);
	LCD_DC_CMD(1);//写数据（DC 拉高，恢复数据模式）
}


/******************************************************************************
      函数说明：设置起始和结束地址（GRAM 访问窗口）
      入口数据：x1,x2 设置列的起始和结束地址
                y1,y2 设置行的起始和结束地址
      返回值：  无
      原理：ST7735S 通过 0x2A（列地址）/0x2B（行地址）划定显存写入窗口，
            再发 0x2C（存储器写）后，连续写入的数据按窗口顺序填充；
            坐标偏移（+1/+2）用于补偿屏边框/偏移参数，不同横竖屏方向偏移不同
******************************************************************************/
void LCD_Address_Set(u16 x1,u16 y1,u16 x2,u16 y2)
{
	if(USE_HORIZONTAL==0)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==1)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+2);
		LCD_WR_DATA(x2+2);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+1);
		LCD_WR_DATA(y2+1);
		LCD_WR_REG(0x2c);//储存器写
	}
	else if(USE_HORIZONTAL==2)
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//储存器写
	}
	else
	{
		LCD_WR_REG(0x2a);//列地址设置
		LCD_WR_DATA(x1+1);
		LCD_WR_DATA(x2+1);
		LCD_WR_REG(0x2b);//行地址设置
		LCD_WR_DATA(y1+2);
		LCD_WR_DATA(y2+2);
		LCD_WR_REG(0x2c);//储存器写
	}
}
//LCD屏初始化
void LCD_Init(void)
{
	SPI2_Init();     //初始化SPI2
	LCD_RES_CMD(0);  //复位（拉低复位引脚）
	Delay_ms(100);
	LCD_RES_CMD(1);  //释放复位（拉高，LCD 开始工作）
	Delay_ms(100);
	
	LCD_BL_CMD(1);	//打开背光
  Delay_ms(100);
	
	//************* Start Initial Sequence **********//
	LCD_WR_REG(0x11); //Sleep out（退出睡眠模式）
	Delay_ms(120);              //Delay 120ms（等待内部稳压器稳定）
	//------------------------------------ST7735S Frame Rate-----------------------------------------// 
	LCD_WR_REG(0xB1); // 帧率控制：设定正常模式刷新频率
	LCD_WR_DATA8(0x05); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_REG(0xB2); // 空闲模式帧率
	LCD_WR_DATA8(0x05);
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_REG(0xB3); // 部分模式帧率
	LCD_WR_DATA8(0x05); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x05); 
	LCD_WR_DATA8(0x3C); 
	LCD_WR_DATA8(0x3C); 
	//------------------------------------End ST7735S Frame Rate---------------------------------// 
	LCD_WR_REG(0xB4); //Dot inversion（点反转：降低闪烁，提升显示质量）
	LCD_WR_DATA8(0x03); 
	//------------------------------------ST7735S Power Sequence---------------------------------// 
	LCD_WR_REG(0xC0); // 电源控制 1：设置 GVDD 电压等
	LCD_WR_DATA8(0x28); 
	LCD_WR_DATA8(0x08); 
	LCD_WR_DATA8(0x04); 
	LCD_WR_REG(0xC1); // 电源控制 2
	LCD_WR_DATA8(0XC0); 
	LCD_WR_REG(0xC2); // 电源控制 3
	LCD_WR_DATA8(0x0D); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_REG(0xC3); // 电源控制 4：VCOM 相关电压
	LCD_WR_DATA8(0x8D); 
	LCD_WR_DATA8(0x2A); 
	LCD_WR_REG(0xC4); // 电源控制 5
	LCD_WR_DATA8(0x8D); 
	LCD_WR_DATA8(0xEE); 
	//---------------------------------End ST7735S Power Sequence-------------------------------------// 
	LCD_WR_REG(0xC5); //VCOM（设置 VCOMH/VCOML 电压）
	LCD_WR_DATA8(0x1A); 
	LCD_WR_REG(0x36); //MX, MY, RGB mode（显存扫描方向控制，决定横/竖屏）
	if(USE_HORIZONTAL==0)LCD_WR_DATA8(0x00);
	else if(USE_HORIZONTAL==1)LCD_WR_DATA8(0xC0);
	else if(USE_HORIZONTAL==2)LCD_WR_DATA8(0x70);
	else LCD_WR_DATA8(0xA0); 
	//------------------------------------ST7735S Gamma Sequence---------------------------------// 
	LCD_WR_REG(0xE0); // 正极性 Gamma 校正曲线（颜色亮度/对比度微调）
	LCD_WR_DATA8(0x04); 
	LCD_WR_DATA8(0x22); 
	LCD_WR_DATA8(0x07); 
	LCD_WR_DATA8(0x0A); 
	LCD_WR_DATA8(0x2E); 
	LCD_WR_DATA8(0x30); 
	LCD_WR_DATA8(0x25); 
	LCD_WR_DATA8(0x2A); 
	LCD_WR_DATA8(0x28); 
	LCD_WR_DATA8(0x26); 
	LCD_WR_DATA8(0x2E); 
	LCD_WR_DATA8(0x3A); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x03); 
	LCD_WR_DATA8(0x13); 
	LCD_WR_REG(0xE1); // 负极性 Gamma 校正曲线
	LCD_WR_DATA8(0x04); 
	LCD_WR_DATA8(0x16); 
	LCD_WR_DATA8(0x06); 
	LCD_WR_DATA8(0x0D); 
	LCD_WR_DATA8(0x2D); 
	LCD_WR_DATA8(0x26); 
	LCD_WR_DATA8(0x23); 
	LCD_WR_DATA8(0x27); 
	LCD_WR_DATA8(0x27); 
	LCD_WR_DATA8(0x25); 
	LCD_WR_DATA8(0x2D); 
	LCD_WR_DATA8(0x3B); 
	LCD_WR_DATA8(0x00); 
	LCD_WR_DATA8(0x01); 
	LCD_WR_DATA8(0x04); 
	LCD_WR_DATA8(0x13); 
	//------------------------------------End ST7735S Gamma Sequence-----------------------------// 
	LCD_WR_REG(0x3A); //65k mode（像素格式：16 位 RGB565）
	LCD_WR_DATA8(0x05); 
	LCD_WR_REG(0x29); //Display on（打开显示）
}


/******************************************************************************
      函数说明：在指定区域填充颜色
      入口数据：xsta,ysta   起始坐标
                xend,yend   终止坐标
								color       要填充的颜色
      返回值：  无
      原理：先划定窗口，再连续写入 (宽x高) 次颜色数据，一次 SPI 突发填满整块区域
******************************************************************************/
void LCD_Fill(u16 xsta, u16 ysta, u16 xend, u16 yend, u16 color)
{          
	u16 num = (xend-xsta) * (yend-ysta);        // 区域内像素总数
	LCD_Address_Set(xsta, ysta, xend-1, yend-1);//设置显示范围
	for(u16 i=0; i<num; i++) {
		LCD_WR_DATA(color);                     // 逐像素写入同一颜色
	}
}

/******************************************************************************
      函数说明：显示图片
      入口数据：x,y起点坐标
                length 图片宽度
                width  图片高度
                pic[]  图片数组（RGB565 数据，2 字节/像素）
      返回值：  无
      原理：图片以 RGB565 数组形式存储在 Flash，这里按顺序读出并写入窗口
******************************************************************************/
void LCD_ShowPicture(u16 x, u16 y, u16 length, u16 width, const u8 pic[])
{
	u16 num = length*width*2;                   // 总字节数 = 像素数 x 2
	u16 color = 0;
	LCD_Address_Set(x, y, x+length-1, y+width-1);
	for(u16 i=0; i<num; i+=2) {
		color = (pic[i]<<8) | pic[i+1];         // 高字节在前拼接成 16 位颜色
		LCD_WR_DATA(color);
	}
}



/******************************************************************************
      函数说明：在指定位置画点
      入口数据：x,y 画点坐标
                color 点的颜色
      返回值：  无
      原理：将窗口设为单点 (x,y)-(x,y)，写入一个颜色即完成画点
******************************************************************************/
void LCD_DrawPoint(u16 x,u16 y,u16 color)
{
	LCD_Address_Set(x,y,x,y);//设置光标位置 
	LCD_WR_DATA(color);      // 写入 1 个像素
} 


/******************************************************************************
      函数说明：画线
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   线的颜色
      返回值：  无
      原理：Bresenham 直线算法——用误差累积代替浮点运算，
            沿增量大的轴步进，另一轴按误差决定是否移动，保证直线连续性
******************************************************************************/
void LCD_DrawLine(u16 x1,u16 y1,u16 x2,u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance;
	int incx,incy,uRow,uCol;
	delta_x=x2-x1; //计算坐标增量 
	delta_y=y2-y1;
	uRow=x1;//画线起点坐标
	uCol=y1;
	if(delta_x>0)incx=1; //设置单步方向 
	else if (delta_x==0)incx=0;//垂直线 
	else {incx=-1;delta_x=-delta_x;}
	if(delta_y>0)incy=1;
	else if (delta_y==0)incy=0;//水平线 
	else {incy=-1;delta_y=-delta_y;}
	if(delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴（增量大的轴）
	else distance=delta_y;
	for(t=0;t<distance+1;t++)
	{
		LCD_DrawPoint(uRow,uCol,color);//画点
		xerr+=delta_x;
		yerr+=delta_y;
		if(xerr>distance)              // 误差累积超过阈值：X 轴前进并回减误差
		{
			xerr-=distance;
			uRow+=incx;
		}
		if(yerr>distance)              // Y 轴同理
		{
			yerr-=distance;
			uCol+=incy;
		}
	}
}


/******************************************************************************
      函数说明：画矩形
      入口数据：x1,y1   起始坐标
                x2,y2   终止坐标
                color   矩形的颜色
      返回值：  无
      原理：由四条直线（上/下/左/右边框）拼接而成
******************************************************************************/
void LCD_DrawRectangle(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	LCD_DrawLine(x1,y1,x2,y1,color);   // 上边
	LCD_DrawLine(x1,y1,x1,y2,color);   // 左边
	LCD_DrawLine(x1,y2,x2,y2,color);   // 下边
	LCD_DrawLine(x2,y1,x2,y2,color);   // 右边
}


/******************************************************************************
      函数说明：画圆
      入口数据：x0,y0   圆心坐标
                r       半径
                color   圆的颜色
      返回值：  无
      原理：中点圆算法——只计算 1/8 圆弧上的点，利用圆的对称性
            同时画出 8 个对称点，a²+b² 与 r² 比较决定是否向内收缩
******************************************************************************/
void Draw_Circle(u16 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	a=0;b=r;	  
	while(a<=b)
	{
		LCD_DrawPoint(x0-b,y0-a,color);             //3           
		LCD_DrawPoint(x0+b,y0-a,color);             //0           
		LCD_DrawPoint(x0-a,y0+b,color);             //1                
		LCD_DrawPoint(x0-a,y0-b,color);             //2             
		LCD_DrawPoint(x0+b,y0+a,color);             //4               
		LCD_DrawPoint(x0+a,y0-b,color);             //5
		LCD_DrawPoint(x0+a,y0+b,color);             //6 
		LCD_DrawPoint(x0-b,y0+a,color);             //7
		a++;
		if((a*a+b*b)>(r*r))//判断要画的点是否过远
		{
			b--;
		}
	}
}

/******************************************************************************
      函数说明：显示汉字串
      入口数据：x,y显示坐标
                *s 要显示的汉字串（GB2312 编码，每字 2 字节）
                fc 字的颜色
                bc 字的背景色
                sizey 字号 可选 12 16 24 32
                mode:  0非叠加模式  1叠加模式
      返回值：  无
      原理：逐汉字调用对应字号显示函数，每显示完一个字 x 坐标右移 sizey 像素
******************************************************************************/
void LCD_ShowChinese(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	while(*s!=0)
	{
		if(sizey==12) LCD_ShowChinese12x12(x,y,s,fc,bc,sizey,mode);
		else if(sizey==16) LCD_ShowChinese16x16(x,y,s,fc,bc,sizey,mode);
//		else if(sizey==24) LCD_ShowChinese24x24(x,y,s,fc,bc,sizey,mode);
//		else if(sizey==32) LCD_ShowChinese32x32(x,y,s,fc,bc,sizey,mode);
		else return;
		s+=2;              // 一个汉字占 2 字节，指针后移
		x+=sizey;          // 下一个字的起始 X 坐标
	}
}

/******************************************************************************
      函数说明：显示单个12x12汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
      原理：点阵字库查找——将汉字内码与字库索引匹配，取出点阵数据 Msk[]；
            每一位对应一个像素，为 1 画前景色，为 0 画背景色（或跳过=叠加模式）
******************************************************************************/
void LCD_ShowChinese12x12(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;   // 每行 8 位存 1 字节，共 sizey 行
	                         
	HZnum=sizeof(tfont12)/sizeof(typFNT_GB12);	//统计汉字数目（字库总字节/单个汉字结构体大小）
	for(k=0;k<HZnum;k++) 
	{
		if((tfont12[k].Index[0]==*(s))&&(tfont12[k].Index[1]==*(s+1)))  // 内码匹配：找到该汉字
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);   // 划定 12x12 显示窗口
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//非叠加方式：每个像素都写颜色（含背景），会覆盖原内容
					{
						if(tfont12[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);   // 点阵位为 1：画前景色
						else LCD_WR_DATA(bc);                            // 点阵位为 0：画背景色
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//叠加方式：仅画前景色点，背景透明，不覆盖原内容
					{
						if(tfont12[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 


/******************************************************************************
      函数说明：显示单个16x16汉字
      入口数据：x,y显示坐标
                *s 要显示的汉字
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
      原理：与 12x12 相同（点阵字库查找），区别是每字 16x16 点阵、占 32 字节，
            Msk 中每个字节的 8 位对应一行中的 8 个像素
******************************************************************************/
void LCD_ShowChinese16x16(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 i,j,m=0;
	u16 k;
	u16 HZnum;//汉字数目
	u16 TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
  TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;  // 16x16：每行 2 字节 x 16 行 = 32 字节
	HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);	//统计汉字数目（字库总字节/单个汉字结构体大小）
	for(k=0;k<HZnum;k++) 
	{
		if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1)))
		{ 	
			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
			for(i=0;i<TypefaceNum;i++)
			{
				for(j=0;j<8;j++)
				{	
					if(!mode)//非叠加方式
					{
						if(tfont16[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
						else LCD_WR_DATA(bc);
						m++;
						if(m%sizey==0)
						{
							m=0;
							break;
						}
					}
					else//叠加方式
					{
						if(tfont16[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
						x++;
						if((x-x0)==sizey)
						{
							x=x0;
							y++;
							break;
						}
					}
				}
			}
		}				  	
		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
	}
} 


///******************************************************************************
//      函数说明：显示单个24x24汉字
//      入口数据：x,y显示坐标
//                *s 要显示的汉字
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void LCD_ShowChinese24x24(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
//{
//	u8 i,j,m=0;
//	u16 k;
//	u16 HZnum;//汉字数目
//	u16 TypefaceNum;//一个字符所占字节大小
//	u16 x0=x;
//	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
//	HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);	//统计汉字数目
//	for(k=0;k<HZnum;k++) 
//	{
//		if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1)))
//		{ 	
//			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
//			for(i=0;i<TypefaceNum;i++)
//			{
//				for(j=0;j<8;j++)
//				{	
//					if(!mode)//非叠加方式
//					{
//						if(tfont24[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
//						else LCD_WR_DATA(bc);
//						m++;
//						if(m%sizey==0)
//						{
//							m=0;
//							break;
//						}
//					}
//					else//叠加方式
//					{
//						if(tfont24[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
//						x++;
//						if((x-x0)==sizey)
//						{
//							x=x0;
//							y++;
//							break;
//						}
//					}
//				}
//			}
//		}				  	
//		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
//	}
//} 

///******************************************************************************
//      函数说明：显示单个32x32汉字
//      入口数据：x,y显示坐标
//                *s 要显示的汉字
//                fc 字的颜色
//                bc 字的背景色
//                sizey 字号
//                mode:  0非叠加模式  1叠加模式
//      返回值：  无
//******************************************************************************/
//void LCD_ShowChinese32x32(u16 x,u16 y,u8 *s,u16 fc,u16 bc,u8 sizey,u8 mode)
//{
//	u8 i,j,m=0;
//	u16 k;
//	u16 HZnum;//汉字数目
//	u16 TypefaceNum;//一个字符所占字节大小
//	u16 x0=x;
//	TypefaceNum=(sizey/8+((sizey%8)?1:0))*sizey;
//	HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);	//统计汉字数目
//	for(k=0;k<HZnum;k++) 
//	{
//		if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1)))
//		{ 	
//			LCD_Address_Set(x,y,x+sizey-1,y+sizey-1);
//			for(i=0;i<TypefaceNum;i++)
//			{
//				for(j=0;j<8;j++)
//				{	
//					if(!mode)//非叠加方式
//					{
//						if(tfont32[k].Msk[i]&(0x01<<j))LCD_WR_DATA(fc);
//						else LCD_WR_DATA(bc);
//						m++;
//						if(m%sizey==0)
//						{
//							m=0;
//							break;
//						}
//					}
//					else//叠加方式
//					{
//						if(tfont32[k].Msk[i]&(0x01<<j))	LCD_DrawPoint(x,y,fc);//画一个点
//						x++;
//						if((x-x0)==sizey)
//						{
//							x=x0;
//							y++;
//							break;
//						}
//					}
//				}
//			}
//		}				  	
//		continue;  //查找到对应点阵字库立即退出，防止多个汉字重复取模带来影响
//	}
//}


/******************************************************************************
      函数说明：显示单个字符（ASCII）
      入口数据：x,y显示坐标
                num 要显示的字符
                fc 字的颜色
                bc 字的背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
      原理：ASCII 点阵字库按字符顺序存放，num-' ' 得到字库偏移；
            宽度为高度一半（如 6x12/8x16），逐字节逐位写出像素
******************************************************************************/
void LCD_ShowChar(u16 x,u16 y,u8 num,u16 fc,u16 bc,u8 sizey,u8 mode)
{
	u8 temp,sizex,t,m=0;
	u16 i,TypefaceNum;//一个字符所占字节大小
	u16 x0=x;
	sizex=sizey/2;   // 字符宽度 = 高度的一半（如 16 号字为 8 像素宽）
	TypefaceNum=(sizex/8+((sizex%8)?1:0))*sizey;
	num=num-' ';    //得到偏移后的值（字库从空格字符开始存放）
	LCD_Address_Set(x,y,x+sizex-1,y+sizey-1);  //设置光标位置 
	for(i=0;i<TypefaceNum;i++)
	{ 
		if(sizey==12)temp=ascii_1206[num][i];		       //调用6x12字体
		else if(sizey==16)temp=ascii_1608[num][i];		 //调用8x16字体
//		else if(sizey==24)temp=ascii_2412[num][i];		 //调用12x24字体
//		else if(sizey==32)temp=ascii_3216[num][i];		 //调用16x32字体
		else return;
		for(t=0;t<8;t++)
		{
			if(!mode)//非叠加模式
			{
				if(temp&(0x01<<t))LCD_WR_DATA(fc);
				else LCD_WR_DATA(bc);
				m++;
				if(m%sizex==0)
				{
					m=0;
					break;
				}
			}
			else//叠加模式
			{
				if(temp&(0x01<<t))LCD_DrawPoint(x,y,fc);//画一个点
				x++;
				if((x-x0)==sizex)
				{
					x=x0;
					y++;
					break;
				}
			}
		}
	}   	 	  	
}


/******************************************************************************
      函数说明：显示字符串
      入口数据：x,y显示坐标
                *p 要显示的字符串
                fc 字的颜色
                bc 背景色
                sizey 字号
                mode:  0非叠加模式  1叠加模式
      返回值：  无
      原理：遍历字符串，逐字符调用 LCD_ShowChar，每显示一个字符 X 坐标右移 sizex
******************************************************************************/
void LCD_ShowString(u16 x,u16 y,const u8 *p,u16 fc,u16 bc,u8 sizey,u8 mode)
{         
	while(*p!='\0')
	{       
		LCD_ShowChar(x,y,*p,fc,bc,sizey,mode);
		x+=sizey/2;    // 字符宽度 = 高度/2
		p++;           // 下一个字符
	}  
}

