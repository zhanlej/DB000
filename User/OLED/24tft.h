#ifndef __24TFT_H__
#define __24TFT_H__

#include "sys.h"


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
#define BROWN						0XBC40 //棕色
#define BRRED						0XFC07 //棕红色
#define GRAY						0X8430 //灰色

#define OLED_LIGHT_TIME 60				//OLED亮屏时间，单位为秒
#define OLED_SWITCH_TIME 2				//OLED切换屏幕时间，单位为秒

						
typedef enum
{
	HINT_STRING_X = 16,
	HINT_STRING_Y = 40,
	PM2_5_X = 16,
	PM2_5_Y = 2,
	LINE_X = 10,
	LINE_Y = 54,
	LINE_WIDTH = 128 - LINE_X*2,
	LINE_HIGH = 1,
	AIR_VOLUM_X = 0,
	AIR_VOLUM_Y = 67,
	VOLUM_WIFI_X = 48,
	VOLUM_WIFI_Y = 62,
	SMALL_MODE_X = 96,
	SMALL_MODE_Y = 62,
	BIG_MODE_X = 40,
	BIG_MODE_Y = 24,
} OLED_PIC_POS_ENUM;	//OLED显示数字的位置，单位像素

typedef enum
{
	NUM_32_48,
	NUM_16_22,
	PICTURE_32_32,
	PICTURE_48_32,
	PICTURE_48_48,
} OLED_PICTURE_TYPE_ENUM;	//OLED显示图片的尺寸

typedef enum
{
	//32*32 and 48*48
	OLED_AUTO_MODE,
	OLED_SLEEP_MODE,
	OLED_SPEED1_MODE,
	OLED_SPEED2_MODE,
	OLED_SPEED3_MODE,
	OLED_POWER_OFF,
	//48*32
	OLED_AIR_VOLUM = 0,
	OLED_AIR_VOLUM_WIFI,
} OLED_PICTURE_ENUM;	//OLED显示图片的类型

typedef enum
{
	UI_CLOSE,
	UI_MAIN,
	UI_WELCOME,
	//下面是切换的界面
	UI_WIFI_STATUS,
	UI_MODE,
} OLED_UI_ENUM;					//OLED显示界面的类型

typedef enum
{
	OLED_AIR_0 = 0,
	OLED_AIR_S = 80,
	OLED_AIR_1 = 120,
	OLED_AIR_2 = 480,
	OLED_AIR_3 = 720,
} OLED_AIR_ENUM;				//OLED显示风量的大小

typedef enum
{
	OLED_WIFI_FAIL,
	OLED_WIFI_OK,
	OLED_WIFI_BREAK_CONNECT,
	OLED_WIFI_AUTO_CONNECT,
	OLED_WIFI_CONNECT_SERVER,
	OLED_WIFI_RESTORE,
	OLED_WIFI_CONFIG,
} OLED_WIFI_STATUS_ENUM;				//OLED显示风量的大小

typedef struct
{
	unsigned int pm2_5;
	OLED_AIR_ENUM air_volum;
	OLED_WIFI_STATUS_ENUM wifi_status;
	OLED_PICTURE_ENUM mode;
} ui_main_t;						//主菜单的结构体

typedef struct
{
	OLED_UI_ENUM ui_type;					//显示的界面
	unsigned char ui_clear;				//是否需要清屏
	unsigned char screen_light;		//是否亮屏
	unsigned int light_time;			//亮屏时间，单位为秒
	unsigned int switch_time;			//屏幕切换时间，单位为秒
	ui_main_t ui_main;						//主菜单显示的内容
} OLED_display_t;								//OLED显示的结构体


extern OLED_display_t OLED_display;


void OLED_init(void);
void OLED_uitype_change(OLED_UI_ENUM ui_type);
void OLED_ui_switch_set(OLED_UI_ENUM ui);							//OLED切换界面设置
unsigned int OLED_switchtime_get(void);								//获取OLED切换界面时间
void OLED_switchtime_set(unsigned int switch_time);		//设置OLED切换界面时间
unsigned int OLED_lighttime_get(void);								//获取OLED亮屏时间
void OLED_lighttime_set(unsigned int light_time);			//设置OLED亮屏时间
unsigned int OLED_screenlight_get(void);							//获取OLED亮屏标志，判断当前是否在息屏状态
void OLED_wifi_status_set(OLED_WIFI_STATUS_ENUM status);
void OLED_mode_change(OLED_PICTURE_ENUM mode);
void OLED_air_set(OLED_AIR_ENUM volum);
void OLED_pm25_set(unsigned int pm2_5);
void OLED_display_handle(void);
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
void LCD_PutNumber(unsigned char x, unsigned char y, int number, OLED_PICTURE_TYPE_ENUM type);
void SPILCD_ShowPicture(unsigned char x,unsigned char y,unsigned int picture, OLED_PICTURE_TYPE_ENUM type);

#endif

