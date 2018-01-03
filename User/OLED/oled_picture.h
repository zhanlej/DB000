#ifndef __OLED_PICTURE_H__
#define __OLED_PICTURE_H__

const unsigned char PictureDot1632[] =              // Pictrue 16*32
{
0x00,0x1C,0x00,0x3E,0x00,0x77,0x00,0x63,0x00,0x43,0x00,0x07,0x00,0x06,0x00,0x1C,0x00,0x1C,0x00,0x1E,0x00,0x07,0x00,0x43,0x00,0x63,0x00,0x76,0x00,0x3E,0x00,0x1C,
0x00,0x00,0x38,0xE0,0x7D,0xF0,0xEF,0xB8,0xC7,0x18,0xC7,0x18,0xC7,0x18,0xC7,0x18,0xC7,0x18,0xC7,0x18,0xC7,0x18,0xC7,0x18,0xC7,0x18,0xC7,0x18,0xC7,0x18,0xC7,0x18,/*"风量",0*/
};

const unsigned char PictureDot3232[] =              // Pictrue 16*32
{
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xF0,0x00,0x00,0x0F,0xFC,0x00,0x00,0xFF,0xFE,0x00,
0x01,0xFF,0xFF,0x00,0x07,0xFF,0x8F,0x80,0x0F,0xF8,0x07,0xE0,0x1F,0x80,0x03,0xF8,0x3C,0x00,0x00,0xFC,0x38,0x00,0x00,0x7C,0x10,0x07,0xE0,0x18,0x00,0x0F,0xF8,0x00,
0x00,0x3F,0xFC,0x00,0x00,0x7C,0x1E,0x00,0x00,0x78,0x0E,0x00,0x00,0x40,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x80,0x00,0x00,0x07,0xC0,0x00,
0x00,0x0F,0xE0,0x00,0x00,0x0F,0xE0,0x00,0x00,0x0F,0xE0,0x00,0x00,0x03,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"wifi成功.BMP",0*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1A,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0xFC,0x00,0x03,0xF3,0xF0,0x00,0x0F,0xFF,0xE0,0x00,0xFF,0xFF,0xC0,
0x01,0xFF,0xFF,0x80,0x07,0xFF,0x8F,0x80,0x0F,0xFB,0x1F,0xE0,0x1F,0x80,0x3F,0xF8,0x3C,0x00,0x7E,0xFC,0x38,0x00,0xFC,0x7C,0x10,0x07,0xF8,0x18,0x00,0x0F,0xF8,0x00,
0x00,0x3F,0xFC,0x00,0x00,0x7F,0xDE,0x00,0x00,0x7F,0xCE,0x00,0x00,0x4F,0x83,0x00,0x00,0x1F,0x00,0x00,0x00,0x3E,0x00,0x00,0x00,0xFD,0x80,0x00,0x01,0xFF,0xC0,0x00,
0x03,0xEF,0xE0,0x00,0x07,0xEF,0xE0,0x00,0x0F,0xCF,0xE0,0x00,0x1F,0x83,0x80,0x00,0x3E,0x00,0x00,0x00,0x1C,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"wifi失败.BMP",0*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0xC0,0x00,0x00,0x3F,0xF0,0x00,0x00,0x78,0x1C,0x00,0x00,0xE0,0x07,0x00,0x01,0x80,0xE1,0x80,
0x03,0x07,0xF0,0xC0,0x06,0x0F,0x30,0x40,0x0E,0x1E,0x28,0x20,0x0C,0x3C,0x18,0x30,0x0C,0x38,0x1C,0x10,0x0C,0x30,0x1C,0x18,0x08,0x50,0x0C,0x18,0x08,0x70,0x0E,0x08,
0x0C,0x7F,0xE6,0x0C,0x0C,0x7F,0xF6,0x0C,0x0C,0x61,0xFF,0x0C,0x14,0x60,0x03,0x04,0x1E,0x40,0x03,0x0C,0x1F,0x40,0x03,0x8C,0x07,0xC0,0x01,0x88,0x03,0xC0,0x00,0x98,
0x01,0xF0,0x00,0x98,0x00,0xFE,0x00,0xB0,0x00,0x3F,0x80,0xE0,0x00,0x0C,0xFF,0xC0,0x00,0x07,0xFF,0xC0,0x00,0x00,0xFF,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"自动模式",0*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x31,0xF0,0x00,0x00,0x7F,0x78,0x00,0x01,0xE0,0x1C,0x00,0x03,0x80,0x0E,0x00,0x07,0x00,0x07,0x00,
0x0E,0x00,0x03,0x80,0x0C,0x00,0x01,0x80,0x18,0x00,0x01,0xC0,0x10,0x00,0x00,0xC0,0x30,0x00,0x00,0x60,0x33,0xE0,0x00,0x60,0x33,0xF8,0x70,0x20,0x31,0xFF,0xF8,0x20,
0x31,0xFF,0xFF,0xA0,0x30,0x00,0x7C,0x20,0x38,0x00,0x00,0x60,0x28,0x00,0x00,0xE0,0x3C,0x00,0x00,0xA0,0x16,0x00,0x01,0xA0,0x1A,0x00,0x03,0x60,0x09,0x00,0x02,0x60,
0x0D,0x80,0x02,0xC0,0x06,0xC0,0x07,0xC0,0x01,0xF0,0x0F,0x80,0x00,0xFF,0xFF,0x00,0x00,0x3F,0xDE,0x00,0x00,0x0F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"睡眠模式",0*/
	
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0F,0x00,0x00,0x00,0x31,0xF0,0x00,0x00,0x7F,0x78,0x00,0x01,0xE0,0x1C,0x00,0x03,0x80,0x0E,0x00,0x07,0x00,0x07,0x00,
0x0E,0x00,0x03,0x80,0x0C,0x00,0x01,0x80,0x18,0x00,0x01,0xC0,0x10,0x00,0x00,0xC0,0x30,0x00,0x00,0x60,0x33,0xE0,0x00,0x60,0x33,0xF8,0x70,0x20,0x31,0xFF,0xF8,0x20,
0x31,0xFF,0xFF,0xA0,0x30,0x00,0x7C,0x20,0x38,0x00,0x00,0x60,0x28,0x00,0x00,0xE0,0x3C,0x00,0x00,0xA0,0x16,0x00,0x01,0xA0,0x1A,0x00,0x03,0x60,0x09,0x00,0x02,0x60,
0x0D,0x80,0x02,0xC0,0x06,0xC0,0x07,0xC0,0x01,0xF0,0x0F,0x80,0x00,0xFF,0xFF,0x00,0x00,0x3F,0xDE,0x00,0x00,0x0F,0xFC,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"一档",0*/
	
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1F,0xC0,0x00,
0x00,0x70,0x78,0x00,0x00,0xC0,0x0F,0x00,0x00,0x80,0x01,0xC0,0x01,0x00,0x00,0x60,0x01,0x00,0x00,0x30,0x03,0x00,0x00,0x10,0x02,0x0F,0xF0,0x18,0x02,0x00,0x18,0x08,
0x06,0x00,0x00,0x08,0x04,0x00,0x00,0x0C,0x04,0x00,0x00,0x04,0x04,0x00,0x00,0x04,0x06,0x00,0x00,0x04,0x02,0x0C,0x0F,0x84,0x03,0x07,0xF8,0x04,0x01,0x80,0x00,0x04,
0x00,0x80,0x00,0x0C,0x00,0xC0,0x00,0x08,0x00,0x60,0x00,0x18,0x00,0x30,0x00,0x70,0x00,0x1C,0x03,0xC0,0x00,0x07,0x03,0x00,0x00,0x01,0xEE,0x00,0x00,0x00,0x38,0x00,/*"二挡",0*/

0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xFF,0xFC,0x00,0x03,0x00,0x03,0x00,
0x06,0x00,0x01,0x80,0x04,0x00,0x00,0x80,0x04,0x1F,0xE0,0xC0,0x04,0x0F,0xF0,0x40,0x04,0x00,0x78,0x40,0x04,0x00,0x00,0x60,0x04,0x00,0x00,0x20,0x04,0x07,0x00,0x20,
0x06,0x07,0xF0,0x20,0x02,0x00,0x38,0x20,0x02,0x00,0x00,0x20,0x03,0x00,0x00,0x30,0x01,0x1F,0x80,0x10,0x01,0x07,0xED,0x90,0x01,0x80,0x7F,0x70,0x00,0x80,0x00,0x10,
0x00,0xC0,0x00,0x20,0x00,0x30,0x7F,0xC0,0x00,0x1C,0x10,0x00,0x00,0x07,0xF0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,/*"三档",0*/
};



#endif

