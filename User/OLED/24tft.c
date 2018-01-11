#include "stdio.h"
#include "24tft.h"
#include "ASCII.h"
#include "GB1616.h"	//16*16汉字字模
#include "oled_number.h"
#include "oled_picture.h"

#define SPILCD_W 176
#define SPILCD_H 240

void ILI9325_CMO24_Initial(void);	//OLED模块初始化代码
void OLED_display_init(void);			//OLED结构体初始化

OLED_display_t OLED_display;

void Delayms(unsigned short time)
{
	unsigned short i,j;
	for(i=0;i<time;i++)
		for(j=0;j<2600;j++)	;
}

void LCD_WriteByteSPI(unsigned char byte)
{
	  unsigned char buf;
    unsigned char i;
    for(i=0;i<8;i++) 
    {
        buf=(byte>>(7-i))&0x1;
        SPI_SDA(buf);
				SPI_DCLK(0);
        SPI_DCLK(1);
    }	
}
void LCD_WriteoneSPI(unsigned char byte)
{
	  unsigned char buf;
    unsigned char i;
	
    for(i=0;i<4;i++) 
    {
        buf=(byte>>(3-i))&0x1;
        SPI_SDA(buf);
				SPI_DCLK(0);
        SPI_DCLK(1);
    }	

}
void WriteComm(unsigned char dat)
{
	SPI_CS(0);
	lcd_RS(0);
	LCD_WriteByteSPI(dat);	//upper eight bits
	lcd_RS(1);
	SPI_CS(1);
}
void LCD_WriteRegIndex(unsigned char Index)
{
	lcd_RS(0);
	LCD_WriteByteSPI(Index);	//upper eight bits
	lcd_RS(1);
}
void LCD_WriteData(unsigned short dat)
{
		lcd_RS(1);
    LCD_WriteByteSPI(dat);
}

//函数名：LCD_WR_REG
//参  数：无
//        准备开始写入GRAM
void SPILCD_WriteRAM_Prepare(void)
{
	LCD_WriteRegIndex(0x22);   //写RAM
}	 

/*************************************************
函数名：Lcd光标起点定位函数
功能：指定320240液晶上的一点作为写数据的起始点
入口参数：x 坐标 0~239
          y 坐标 0~319
返回值：无
*************************************************/
void Lcd_SetCursor(u16 x,u16 y)
{ 
  LCD_WriteRegIndex(0x20);
  LCD_WriteData(x);//水平坐标
  LCD_WriteRegIndex(0x21);
  LCD_WriteData(y);//垂直坐标 
} 

//函数名：LCD_WR_REG
//参  数：无
//        写GRAM数据
void SPILCD_WriteRAM(unsigned short RGB_Code)
{							    
    LCD_WriteData(RGB_Code); 
}
//函数名：LCD_SetWindow
//参  数：Xpos:横坐标
//				Ypos:纵坐标
void SPILCD_SetWindow(unsigned short xstat,unsigned short xend,unsigned short ystat,unsigned short yend)
{
	//HX8367-A

	//Set GRAM Area 
	LCD_WR_REG(0x02,xstat>>8); 
	LCD_WR_REG(0x03,xstat&0xff);     //Column Start 
	LCD_WR_REG(0x04,xend>>8); 
	LCD_WR_REG(0x05,xend&0xff);     //Column End 
	 
	LCD_WR_REG(0x06,ystat>>8); 
	LCD_WR_REG(0x07,ystat&0xff);     //Row Start 
	LCD_WR_REG(0x08,yend>>8); 
	LCD_WR_REG(0x09,yend&0xff);     //Row End 
	  
	 LCD_WriteRegIndex(0x22);

}
//开启反色显示
void LCD_InvDisplayOn()
{
  LCD_WriteRegIndex(0x21);  
}
//关闭反色显示
void LCD_InvDisplayOff()
{
  LCD_WriteRegIndex(0x20); 
}

/******************************************
函数名：Lcd写命令函数
功能：向Lcd指定位置写入应有命令或数据
入口参数：Index 要寻址的寄存器地址
          ConfigTemp 写入的数据或命令值
******************************************/
void LCD_WR_REG(u16 Index,u16 CongfigTemp)
{
	LCD_WriteRegIndex(Index);
	LCD_WriteByteSPI(CongfigTemp);
}

void ILI9325_CMO24_Initial(void)
{
	//复位
	GPIO_ResetBits(GPIOA, GPIO_Pin_2);Delayms(100);
	GPIO_SetBits(GPIOA, GPIO_Pin_2);Delayms(100);
	
	SPI_CS(0);
	Delayms(25);

	WriteComm(0xae);

	WriteComm(0x15);    //   设置列地址
	WriteComm(0x00);    //  start column   0
	WriteComm(0x3f);    //  end column   127

	WriteComm(0x75);    //   设置行地址
	WriteComm(0x00);    //  start row   0
	WriteComm(0x7f);    //  end row   127

	WriteComm(0x81);	 // 设置对比度
	WriteComm(0x80);


	WriteComm(0xa0);    //segment remap
	WriteComm(0x42);	  //

	WriteComm(0xa1);	 // start line
	WriteComm(0x00);

	WriteComm(0xa2);	 // display offset
	WriteComm(0x60);

	WriteComm(0xa4);    //normal display

	WriteComm(0xa8);    // set multiplex ratio
	WriteComm(0x7f);

	WriteComm(0xb1);	 // set phase leghth
	WriteComm(0xf1);

	WriteComm(0xb3);  // set dclk
	WriteComm(0x00);  //80Hz:0xc1	90Hz:0xe1  100Hz:0x00	110Hz:0x30 120Hz:0x50 130Hz:0x70

	WriteComm(0xab);	 // 
	WriteComm(0x01);	 //


	/*WriteComm(0xb8);
	WriteComm(0x00);
	WriteComm(0x08);
	WriteComm(0x10);
	WriteComm(0x18);
	WriteComm(0x20);
	WriteComm(0x28);
	WriteComm(0x30);
	WriteComm(0x38);*/

	WriteComm(0xb6);	 // set phase leghth
	WriteComm(0x0f);

	WriteComm(0xbe);
	WriteComm(0x0f);

	WriteComm(0xbc);
	WriteComm(0x08);

	WriteComm(0xd5);
	WriteComm(0x62);

	WriteComm(0xfd);
	WriteComm(0x12);

	WriteComm(0xaf); 
}

//函数名：SPILCD_Init
//参  数：X Y 坐标 
//        在X Y上打点
void SPILCD_DrawPoint(unsigned short x,unsigned short y,unsigned short color)
{
	SPILCD_SetWindow(x,x+1,y,y+1);//设置光标位置 
	SPILCD_WriteRAM_Prepare();     //开始写入GRAM	 
	SPILCD_WriteRAM(color);
}
//void SPILCD_Fill(unsigned short xsta,unsigned short ysta,unsigned short xend,unsigned short yend,unsigned short color)
//{                    
//	unsigned short i,j;
//	//设置窗口		
//	SPILCD_SetWindow(xsta,xend,ysta,yend);
//	for(i=xsta;i<=xend;i++)
//		for(j=ysta;j<=yend;j++)
//	{
//		   	SPILCD_WriteRAM(color);	  //显示所填充的颜色. 
//// 		k=40000;while(k--);
//	}
//	//恢复设置
//	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	    
//}
//函数名：SPILCD_Clear
//参  数：Color 颜色      
void SPILCD_Clear(unsigned short Color)
{
	unsigned short x,y;  
	WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(0x00); 
	WriteComm(127); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(0x00); 
	WriteComm(95); 	
//	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	
SPI_CS(0);	
	for(x=0;x<128;x++)
		for(y=0;y<48;y++)
	{
		SPILCD_WriteRAM(Color);//显示所填充的颜色. 
// 		i=40000;while(i--);
	}
	SPI_CS(1);
} 
//函数名：SPILCD_Clear_Fast
//参  数：single_Color 单色  只能是0或1，0是黑色，1为白色      
void SPILCD_Clear_Fast(unsigned char single_Color)
{
	unsigned int x,y;
  int temp;	
	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	
	SPI_SDA(single_Color);
	lcd_RS(1);
	temp=SPILCD_W<<4;
	for(x=0;x<temp;x++)
		for(y=0;y<SPILCD_H;y++)
	{
    SPI_DCLK(0);
    SPI_DCLK(1);
	}
} 
//------------------------------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------------------------------
//SPILCD DRIVER  LV2层    ----   LV1 在LV0的基础上实现
//单字符显示
//字符串显示
//数字显示

//函数名：SPILCD_ShowChar
//参  数：
//(x,y): 
//num:要显示的字符:" "--->"~"
//size:字体大小 12/16
//mode:叠加方式(1)还是非叠加方式(0)
void SPILCD_ShowChar(unsigned char x,unsigned char y,unsigned char num)
{       
	unsigned char temp;
	unsigned int pos,t,i;  
	unsigned char size; 
	size=16;	//找到字体大小
  WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(x); 
	WriteComm(x+3); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(0x0+y); 
	WriteComm(0x0+y+15); 
	lcd_RS(1);
	SPI_CS(0);  
	num=num-' ';//得到偏移后的值
	i=num*16;

		for(pos=0;pos<size;pos++)
		{

			temp=nAsciiDot[i+pos];	//调通调用艺术字体
			for(t=0;t<8;t++)
		   {                 
		      if(temp&0x80)
						LCD_WriteoneSPI(0xff);
					else 
						LCD_WriteoneSPI(0x00);
		      temp<<=1; 
		    }
		}	 
}  
void DrawPixel(u16 x, u16 y, int Color)
{
	SPILCD_SetWindow(x,x,y,y); 
  SPILCD_WriteRAM(Color);							  
}
void PutGB1616(unsigned char x, unsigned char  y, unsigned char c[2])
{
	unsigned int i,j,k;
	unsigned short m;
	WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(0x0+x); 
	WriteComm(0x0+x+7); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(0x0+y); 
	WriteComm(0x0+y+15); 
//	SPILCD_SetWindow(0,SPILCD_W-1,0,SPILCD_H-1);	
	lcd_RS(1);
SPI_CS(0);	

// 	SPILCD_SetWindow(0,SPILCD_H-1,0,SPILCD_W-1);
	for (k=0;k<64;k++) { //64标示自建汉字库中的个数，循环查询内码
	  if ((codeGB_16[k].Index[0]==c[0])&&(codeGB_16[k].Index[1]==c[1]))
			{ 
    	for(i=0;i<32;i++) 
			{
				m=codeGB_16[k].Msk[i];
				for(j=0;j<8;j++) 
				{		
					if((m&0x80)==0x80) {
						LCD_WriteoneSPI(0xff);
						}
					else {
						LCD_WriteoneSPI(0x00);
						}
					m=m<<1;
				} 
				if(i%2){y++;x=x-8;}
				else x=x+8;
		  }
		}  
	  }	
	}
void LCD_PutString(unsigned char x, unsigned char y, unsigned char *s) 
{
	unsigned char l=0;
	while(*s) 
		{
			if( *s < 0x80) 
				{
					SPILCD_ShowChar(x+l,y,*s);
					s++;l+=4;
				}
			else
				{
					PutGB1616(x+l,y,(unsigned char*)s);
					s+=2;l=l+8;
				}
		}
}

void SPILCD_WriteArray(unsigned int start, unsigned int size, const unsigned char * array)
{
	int pos, t;
	unsigned char temp;
	
	for(pos=0;pos<size;pos++)
	{
		temp=array[start+pos];	//调通调用艺术字体
		for(t=0;t<8;t++)
		{                 
			if(temp&0x80)
				LCD_WriteoneSPI(0xff);
			else 
				LCD_WriteoneSPI(0x00);
			temp<<=1; 
		}
	}
}

void SPILCD_ShowPicture(unsigned char x,unsigned char y,unsigned int picture, OLED_PICTURE_TYPE_ENUM type)
{       
	unsigned int i;  
	unsigned char width, high;
	unsigned int size = 0;
	unsigned char * array_p;
	
	switch(type)
	{
		case NUM_32_48:
			width = 32;	high = 48;
			array_p = (unsigned char *)NumberDot3248;
			break;
		case NUM_16_22:
			width = 16;	high = 22;
			array_p = (unsigned char *)NumberDot1622;
			break;
		case PICTURE_32_32:
			width = 32;	high = 32;
			array_p = (unsigned char *)PictureDot3232;
			break;
		case PICTURE_48_32:
			width = 48;	high = 32;
			array_p = (unsigned char *)PictureDot4832;
			break;
		case PICTURE_48_48:
			width = 48;	high = 48;
			array_p = (unsigned char *)PictureDot4848;
			break;
	}
	
	size = high*(width/8);
	
  WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(x/2); 
	WriteComm((x+width)/2-1); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(0x0+y); 
	WriteComm(0x0+y+high-1); 
	lcd_RS(1);
	SPI_CS(0);  
	//num=num-'0';//得到偏移后的值
	i=picture*size;
	
	SPILCD_WriteArray(i, size, array_p);
	
}

void LCD_PutNumber(unsigned char x, unsigned char y, int number, OLED_PICTURE_TYPE_ENUM type) 
{
	unsigned char l=0;
	unsigned char i=0;
	unsigned char temp[3];
	unsigned char width;
	
	if(number > 999)						//最大显示数值为999
		number = 999;
	
	if(type == NUM_32_48)
		width = 32;
	else if(type == NUM_16_22)
		width = 16;
	
	temp[0] = number/100%10;
	temp[1] = number/10%10;
	temp[2] = number%10;
	
	for(i = 0; i < 3; i++)
	{
		SPILCD_ShowPicture(x+l, y, temp[i], type);
		l += width;
	}
}

/******************************************
函数名：Lcd图像填充100*100
功能：向Lcd指定位置填充图像
入口参数：
******************************************/
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic)
{
  unsigned long i;
//	unsigned int j;
	SPILCD_SetWindow(x,x+pic_H-1,y,y+pic_V-1);

// 	lcd_RS(1);
	for (i = 0; i < pic_H*pic_V*2; i++)
	{
    LCD_WriteByteSPI(pic[i]);
	}
// 	SPILCD_SetWindow(0,319,0,239);//写完图片后恢复整个显示区域

}

void OLED_init(void)
{
	//GPIO初始化在main函数中
	ILI9325_CMO24_Initial();				//OLED模块初始化代码
	OLED_display_init();
}

void OLED_display_init(void)
{
	OLED_display.ui_type = UI_WELCOME;
	OLED_display.ui_clear = 0;
	OLED_display.screen_light = 1;
	OLED_display.light_time = OLED_LIGHT_TIME;
	OLED_display.switch_time = 0;
	OLED_display.ui_main.pm2_5 = 0;
	OLED_display.ui_main.air_volum = OLED_AIR_0;
	OLED_display.ui_main.wifi_status = OLED_WIFI_FAIL;
	OLED_display.ui_main.mode = OLED_AUTO_MODE;
}

void OLED_uitype_change(OLED_UI_ENUM ui_type)
{
	OLED_display.ui_type = ui_type;
	OLED_display.ui_clear = 1;				//需要清屏操作
}

void OLED_ui_switch_set(OLED_UI_ENUM ui)	//OLED切换界面设置
{
	if(ui == UI_WIFI_STATUS || ui == UI_MODE)		//只有这两种情况是需要界面切换的
	{
		OLED_uitype_change(ui);	//OLED切换到对应的界面
		OLED_display.switch_time = OLED_SWITCH_TIME;
		OLED_display_handle();
	}
}

unsigned int OLED_switchtime_get(void)		//获取OLED切换界面时间
{
	return OLED_display.switch_time;
}

void OLED_switchtime_set(unsigned int switch_time)	//设置OLED切换界面时间
{
	OLED_display.switch_time = switch_time;
	if(OLED_display.switch_time == 0)	//如果切屏时间到后立即刷新OLED界面
		OLED_display_handle();
}

void OLED_wifi_status_set(OLED_WIFI_STATUS_ENUM status)
{
	OLED_display.ui_main.wifi_status = status;
	OLED_ui_switch_set(UI_WIFI_STATUS);
}

void OLED_mode_change(OLED_PICTURE_ENUM mode)
{
	if(mode == OLED_POWER_OFF)	//在关机模式不需要设置OLED上mode图案
	{
		OLED_display_handle();
		return;
	}
	OLED_display.ui_main.mode = mode;
	OLED_ui_switch_set(UI_MODE);
}

void OLED_air_set(OLED_AIR_ENUM volum)
{
	OLED_display.ui_main.air_volum = volum;
}

void OLED_pm25_set(unsigned int pm2_5)
{
	OLED_display.ui_main.pm2_5 = pm2_5;
}

void OLED_display_handle(void)
{
	static unsigned char wifi_flag = 0;
	
	if(OLED_display.ui_clear)
	{
		OLED_display.ui_clear = 0;
		SPILCD_Clear(0x00);							//OLED清屏
	}
	
	switch(OLED_display.ui_type)
	{
		case UI_CLOSE:
			break;
		case UI_MAIN:
			LCD_PutNumber(PM2_5_X,PM2_5_Y,OLED_display.ui_main.pm2_5,NUM_32_48);	//PM2.5
			SPILCD_Fill(LINE_X,LINE_Y,LINE_WIDTH,LINE_HIGH,0xff);
			LCD_PutNumber(AIR_VOLUM_X,AIR_VOLUM_Y,OLED_display.ui_main.air_volum,NUM_16_22);
			//SPILCD_ShowPicture(48,64,OLED_AIR_VOLUM,PICTURE_16_32);
			if(OLED_display.ui_main.wifi_status == OLED_WIFI_OK)
				SPILCD_ShowPicture(VOLUM_WIFI_X,VOLUM_WIFI_Y,OLED_AIR_VOLUM_WIFI,PICTURE_48_32);
			else
			{
				if(wifi_flag == 0)
				{
					wifi_flag = 1;
					//没有wifi的图案
					SPILCD_ShowPicture(VOLUM_WIFI_X,VOLUM_WIFI_Y,OLED_AIR_VOLUM,PICTURE_48_32);
				}
				else
				{
					wifi_flag = 0;
					SPILCD_ShowPicture(VOLUM_WIFI_X,VOLUM_WIFI_Y,OLED_AIR_VOLUM_WIFI,PICTURE_48_32);
				}
			}
			SPILCD_ShowPicture(SMALL_MODE_X,SMALL_MODE_Y,OLED_display.ui_main.mode,PICTURE_32_32);
			break;
		case UI_WELCOME:
			LCD_PutString(HINT_STRING_X,HINT_STRING_Y,"质享科技");
			break;
		case UI_WIFI_STATUS:
			if(OLED_display.ui_main.wifi_status == OLED_WIFI_OK || OLED_display.ui_main.wifi_status == OLED_WIFI_FAIL)
			{
				if(OLED_display.switch_time)						//切换界面的时间还没到
				{			
					switch(OLED_display.ui_main.wifi_status)
					{
						case OLED_WIFI_OK:
							LCD_PutString(HINT_STRING_X,HINT_STRING_Y,"连接成功");
							break;
						case OLED_WIFI_FAIL:
							LCD_PutString(HINT_STRING_X,HINT_STRING_Y,"连接失败");
							break;
						default:
							break;
					}				
				}
				else																		//到时间后判断如果在关机状态就切换到关屏界面，否则切换到主菜单界面
				{
					if(OLED_display.ui_main.air_volum == OLED_AIR_0)
					{
						OLED_uitype_change(UI_CLOSE);
					}
					else
						OLED_uitype_change(UI_MAIN);		//OLED切换到主界面
				}
			}
			else
			{
				switch(OLED_display.ui_main.wifi_status)
					{
						case OLED_WIFI_AUTO_CONNECT:
							LCD_PutString(HINT_STRING_X,HINT_STRING_Y,"尝试连接");
							break;
						case OLED_WIFI_CONNECT_SERVER:
							LCD_PutString(HINT_STRING_X,HINT_STRING_Y,"连服务器");
							break;
						case OLED_WIFI_RESTORE:
							LCD_PutString(HINT_STRING_X,HINT_STRING_Y,"wifi重置");
							break;
						case OLED_WIFI_CONFIG:
							LCD_PutString(HINT_STRING_X,HINT_STRING_Y,"正在配网");
							break;
						default:
							break;
					}				
			}
			break;
		case UI_MODE:
			if(OLED_display.switch_time)
			{
				switch(OLED_display.ui_main.mode)
				{
					case OLED_AUTO_MODE:
						SPILCD_ShowPicture(BIG_MODE_X,BIG_MODE_Y,OLED_AUTO_MODE,PICTURE_48_48);
						break;
					case OLED_SLEEP_MODE:
						SPILCD_ShowPicture(BIG_MODE_X,BIG_MODE_Y,OLED_SLEEP_MODE,PICTURE_48_48);
						break;
					case OLED_SPEED1_MODE:
						SPILCD_ShowPicture(BIG_MODE_X,BIG_MODE_Y,OLED_SPEED1_MODE,PICTURE_48_48);
						break;
					case OLED_SPEED2_MODE:
						SPILCD_ShowPicture(BIG_MODE_X,BIG_MODE_Y,OLED_SPEED2_MODE,PICTURE_48_48);
						break;
					case OLED_SPEED3_MODE:
						SPILCD_ShowPicture(BIG_MODE_X,BIG_MODE_Y,OLED_SPEED3_MODE,PICTURE_48_48);
						break;
					default:
						break;
				}
			}
			else
			{
				if(OLED_display.ui_main.wifi_status != OLED_WIFI_FAIL && OLED_display.ui_main.wifi_status != OLED_WIFI_OK)	//如果在连网状态，模式界面显示完后会切换回连网界面
					OLED_ui_switch_set(UI_WIFI_STATUS);
				else
					OLED_uitype_change(UI_MAIN);		//OLED切换到主界面
			}
			break;
	}
}

void SPILCD_Fill(unsigned short xsta,unsigned short ysta,unsigned short xlen,unsigned short ylen,unsigned short color)
{       
	int i,j;
	
  WriteComm(0x15);//SET COLUMN ADDR 
	WriteComm(xsta/2); 
	WriteComm((xsta+xlen)/2-1); 
	WriteComm(0x75);//SET ROW ADDR 
	WriteComm(ysta); 
	WriteComm(ysta+ylen-1);  

	SPI_CS(0);	
	for(i=0/2;i<(xlen/2);i++)
	{
		for(j=0;j<ylen;j++)
		{
			SPILCD_WriteRAM(color);//显示所填充的颜色. 
//	 		i=40000;while(i--);
		}
	}		
	SPI_CS(1);
}
