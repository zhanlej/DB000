#include "stm32f10x.h"

#define WS2812B_RED 	0xFF00		//红光
#define WS2812B_GREEN	0xFF0096	//绿光
#define WS2812B_BLUE	0xFF			//蓝光
#define WS2812B_WHITE	0xFFFFFF	//白光
#define WS2812B_ORANGE 0x5AFF00	//橙光
#define WS2812B_CUTDOWN	0x000000	//关闭显示
#define WS2812B_TESTCOLOR 0xAAAAAA	//测试颜色
//#define RED 	0xFF00		//红光
//#define GREEN	0x9600FF	//绿光
//#define BLUE	0xFF0000			//蓝光
//#define WHITE	0xFFFFFF	//白光
//#define ORANGE 0xFF5A	//橙光
//#define CUTDOWN	0x000000	//关闭显示
//#define TESTCOLOR 0xAAAAAA	//测试颜色

void RGB_Set_Up(void);
void RGB_Set_Down(void);
void RGB_Set(u32 G8R8B8, int len);
void RGB_Rst(void);

