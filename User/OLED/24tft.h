#ifndef __24TFT_H__
#define __24TFT_H__

#include "sys.h"

typedef enum
{
	NUM_40_64,
	NUM_16_32,
} OLED_NUM_TYPE_ENUM;

typedef enum
{
	PICTURE_16_32,
	PICTURE_32_32,
} OLED_PICTURE_TYPE_ENUM;

typedef enum
{
	OLED_WIFI_OK,
	OLED_WIFI_FAIL,
	OLED_AUTO_MODE,
	OLED_SLEEP_MODE,
	OLED_SPEED1_MODE,
	OLED_SPEED2_MODE,
	OLED_SPEED3_MODE,
	
	OLED_AIR_VOLUM = 0,
} OLED_PICTURE_ENUM;

#define lcd_RS(a)	\
						if (a)	\
						GPIOB->BSRR = GPIO_Pin_4;	\
						else		\
						GPIOB->BRR = GPIO_Pin_4;
#define SPI_CS(a)	\
						if (a)	\
						GPIOB->BSRR = GPIO_Pin_5;	\
						else		\
						GPIOB->BRR = GPIO_Pin_5;
#define SPI_DCLK(a)	\
						if (a)	\
						GPIOB->BSRR = GPIO_Pin_13;	\
						else		\
						GPIOB->BRR = GPIO_Pin_13;
#define SPI_SDA(a)	\
						if (a)	\
						GPIOB->BSRR = GPIO_Pin_15;	\
						else		\
						GPIOB->BRR = GPIO_Pin_15;


#define WHITE						0xFFFF
#define BLACK						0x0000	  
#define BLUE						0x001F  
#define BRED						0XF81F
#define GRED						0XFFE0
#define GBLUE						0X07FF
#define RED							0xF800
#define MAGENTA						0xF81F
#define GREEN						0x07E0
#define CYAN						0x7FFF
#define YELLOW						0xFFE0
#define BROWN						0XBC40 //×ØÉ«
#define BRRED						0XFC07 //×ØºìÉ«
#define GRAY						0X8430 //»ÒÉ«

void ILI9325_CMO24_Initial(void);
void Delayms(unsigned short time);
void LCD_WriteRegIndex(unsigned char Index);
void LCD_WriteData(unsigned short dat);
void LCD_WR_REG(u16 Index,u16 CongfigTemp);
void Lcd_SetCursor(u16 x,u16 y);
void SPILCD_SetWindow(unsigned short xstat,unsigned short xend,unsigned short ystat,unsigned short yend);
void SPILCD_DrawPoint(unsigned short x,unsigned short y,unsigned short color);
void SPILCD_Clear(unsigned short Color);
void SPILCD_Clear_Fast(unsigned char single_Color);
void SPILCD_Fill(unsigned short xsta,unsigned short ysta,unsigned short xend,unsigned short yend,unsigned short color);
void SPILCD_DrawLine(unsigned short x1,unsigned short y1,unsigned short x2,unsigned short y2,unsigned short color);
void SPILCD_ShowChar(unsigned char x,unsigned char y,unsigned char num);
void LCD_PutString(unsigned char x, unsigned char y, unsigned char *s);
void LCD_Fill_Pic(u16 x, u16 y,u16 pic_H, u16 pic_V, const unsigned char* pic);
void LCD_PutNumber(unsigned char x, unsigned char y, int number, OLED_NUM_TYPE_ENUM type);
void SPILCD_ShowPicture(unsigned char x,unsigned char y,OLED_PICTURE_ENUM picture, OLED_PICTURE_TYPE_ENUM type);

#endif

