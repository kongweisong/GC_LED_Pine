/*
 * tft.c
 *
 *  Created on: Apr 27, 2025
 *      Author: zhudi
 */

#include "tft.h"
#include "font.h"
#include "gpio.h"
#include "spi.h"
#include "key.h"
#include "usart_control.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#define TFT_COLUMN_NUMBER 160
#define TFT_LINE_NUMBER 128
#define TFT_COLUMN_OFFSET 0
#define TFT_LINE_OFFSET 0
extern SystemDef sys;


uint32_t DMA1_MEM_LEN;	//保存DMA每次数据传送的长度
uint8_t SendBuff[2*TFT_COLUMN_NUMBER];


void tft_reset(void)
{
	tft_rst_0();
	HAL_Delay(20);
	tft_rst_1();
	HAL_Delay(120);
}
void tft_send_cmd(uint8_t o_command)
{
	tft_dc_0();	
	HAL_SPI_Transmit_DMA(&hspi1, &o_command, 1);	
	tft_dc_1();	
}
void tft_send_data(uint8_t o_data)
{
	HAL_SPI_Transmit_DMA(&hspi1, &o_data, 1);
}
void tft_send_data_u16(uint16_t temp)
{
	uint8_t tempBuf[2];
	tempBuf[0] = temp >> 8;
	tempBuf[1] = temp;
	HAL_SPI_Transmit_DMA(&hspi1, tempBuf, 2);
}
void tft_address_set(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end)
{
	tft_send_cmd(0x2a);	 // Column address set
	tft_send_data(0x00); // start column
	tft_send_data(x_start);
	tft_send_data(0x00); // end column
	tft_send_data(x_end);

	tft_send_cmd(0x2b);	 // Row address set
	tft_send_data(0x00); // start row
	tft_send_data(y_start);
	tft_send_data(0x00); // end row
	tft_send_data(y_end);
	tft_send_cmd(0x2C); // Memory write
}
void tft_full(unsigned long color)
{
    uint16_t i, j;
    uint8_t buffer[TFT_COLUMN_NUMBER * 2];
    
    uint8_t high_byte = (color >> 8);
    uint8_t low_byte = color;
    for(i = 0; i < TFT_COLUMN_NUMBER * 2; i += 2)
    {
        buffer[i] = high_byte;
        buffer[i + 1] = low_byte;
    }
    tft_address_set(0, 0, TFT_COLUMN_NUMBER, TFT_LINE_NUMBER);
    
    for(j = 0; j < TFT_LINE_NUMBER; j++)
    {
        HAL_SPI_Transmit_DMA(&hspi1, buffer, TFT_COLUMN_NUMBER * 2);
        // 等待DMA传输完成
        while(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
    }
}
void tft_drawLine(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t color)
{
	int16_t dx = abs(x1 - x0);
	int16_t dy = -abs(y1 - y0);
	int16_t sx = x0 < x1 ? 1 : -1;
	int16_t sy = y0 < y1 ? 1 : -1;
	int16_t err = dx + dy;
	int16_t e2;

	while (1)
	{
		// 绘制当前点
		tft_address_set(x0, y0, x0, y0);
		tft_send_data(color >> 8);
		tft_send_data(color & 0xFF);

		if (x0 == x1 && y0 == y1)
			break;

		e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			x0 += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			y0 += sy;
		}
	}
}
void display_char12_12(unsigned int x, unsigned int y, unsigned long fg_color, unsigned long bg_color, const unsigned char *point)
{
	uint8_t column;
	uint8_t tm, temp;

	// 1. 设置显示区域（16x16像素）
	tft_address_set(x, y, x + 15, y + 15);

	// 2. 逐像素绘制
	for (column = 0; column < 24; column++)
	{					 // 32字节（16x16点阵）
		temp = *point++; // 读取当前字节
		for (tm = 0; tm < 8; tm++)
		{ // 处理每个比特（LSB优先）
			if (temp & 0x01)
			{
				// 前景色（1）：发送fg_color
				tft_send_data(fg_color >> 8); // 高8位
				tft_send_data(fg_color);	  // 低8位
			}
			else
			{
				// 背景色（0）：发送bg_color（非透明模式）
				if (bg_color != 0xFFFF)
				{
					tft_send_data(bg_color >> 8);
					tft_send_data(bg_color);
				}
				// 透明模式：不发送数据（保留屏幕原内容）
			}
			temp >>= 1; // 右移处理下一个比特
		}
	}
}
void tft_init(void)
{
	HAL_Delay(500);
	tft_reset();
	tft_send_cmd(0x11); // Sleep Out
#ifdef TFT_USE_RTOS
	osDelay(120);
#else
	HAL_Delay(120);
#endif

	tft_send_cmd(0xF0);
	tft_send_cmd(0xD6);
	tft_send_data(0xCB);

	tft_send_cmd(0xB1);
	tft_send_data(0x01); // 05
	tft_send_data(0x04); // 3C
	tft_send_data(0x01); // 3C

	tft_send_cmd(0xB2);
	tft_send_data(0x05);
	tft_send_data(0x3C);
	tft_send_data(0x3C);

	tft_send_cmd(0xB3);
	tft_send_data(0x05);
	tft_send_data(0x3C);
	tft_send_data(0x3C);
	tft_send_data(0x05);
	tft_send_data(0x3C);
	tft_send_data(0x3C);

	tft_send_cmd(0xB4); // Dot inversion
	tft_send_data(0x03);
	tft_send_data(0x02);

	tft_send_cmd(0xC0);
	tft_send_data(0x64); // 改善灰底crosstalk
	tft_send_data(0x04);
	tft_send_data(0x84);

	tft_send_cmd(0xC1);
	tft_send_data(0xC4);

	tft_send_cmd(0xC2);
	tft_send_data(0x0D);
	tft_send_data(0x00);

	tft_send_cmd(0xC3);
	tft_send_data(0x8D);
	tft_send_data(0x2A);

	tft_send_cmd(0xC4);
	tft_send_data(0x8D);
	tft_send_data(0xEE);

	tft_send_cmd(0xC5);
	tft_send_data(0x01);

	tft_send_cmd(0xE0);
	tft_send_data(0x08);
	tft_send_data(0x19);
	tft_send_data(0x1B);
	tft_send_data(0x1E);
	tft_send_data(0x3F);
	tft_send_data(0x38);
	tft_send_data(0x30);
	tft_send_data(0x32);
	tft_send_data(0x2E);
	tft_send_data(0x29);
	tft_send_data(0x30);
	tft_send_data(0x38);
	tft_send_data(0x00);
	tft_send_data(0x09);
	tft_send_data(0x00);
	tft_send_data(0x10);

	tft_send_cmd(0xE1);
	tft_send_data(0x07);
	tft_send_data(0x16);
	tft_send_data(0x13);
	tft_send_data(0x14);
	tft_send_data(0x36);
	tft_send_data(0x2F);
	tft_send_data(0x29);
	tft_send_data(0x2A);
	tft_send_data(0x2A);
	tft_send_data(0x28);
	tft_send_data(0x2F);
	tft_send_data(0x3D);
	tft_send_data(0x00);
	tft_send_data(0x04);
	tft_send_data(0x02);
	tft_send_data(0x10);

	tft_send_cmd(0x2A);
	tft_send_data(0x00);
	tft_send_data(0x00);
	tft_send_data(0x00);
	tft_send_data(0x7F);

	tft_send_cmd(0x2B);
	tft_send_data(0x00);
	tft_send_data(0x00);
	tft_send_data(0x00);
	tft_send_data(0x9F);

	tft_send_cmd(0x35);
	tft_send_data(0x00);

	tft_send_cmd(0x3A); // 65k mode
	tft_send_data(0x05);

	tft_send_cmd(0x36);
	tft_send_data(0x60);

	tft_send_cmd(0x21);

	tft_send_cmd(0x29); // Display on

	tft_send_cmd(0x2C);
}
void tft_ShowChar12_12(uint16_t x, uint16_t y, char ch, uint16_t font_color, uint16_t back_color)
{
	int i = 0, j = 0;
	uint8_t temp = 0;
	uint8_t size = 0;
	tft_address_set(x, y, x + 12 / 2 - 1, y + 12 - 1);
	ch = ch - ' ';
	size = (12 / 8 + ((12 % 8) ? 1 : 0)) * (12 / 2);
	for (i = 0; i < size; i++)
	{
		temp = asc2_1206[ch][i];
		for (j = 0; j < 6; j++)
		{
			if (temp & 0x80)
			{
				tft_send_data_u16(font_color);
			}
			else
			{
				tft_send_data_u16(back_color);
			}

			temp <<= 1;
		}
	}
}
void tft_ShowStr12_12(uint16_t x, uint16_t y, uint32_t max_width, char *str, uint16_t font_color, uint16_t back_color)
{
	max_width += x;
	while ((*str <= '~') && (*str >= ' '))
	{
		if (x >= max_width)
		{
			break;
		}
		tft_ShowChar12_12(x, y, *str, font_color, back_color);
		x += 12 / 2;
		str++;
	}
}
void tft_ShowChar14_14(uint16_t x, uint16_t y, char ch, uint16_t font_color, uint16_t back_color)
{
	int i = 0, j = 0;
	uint8_t temp = 0;
	uint8_t size = 0;
	tft_address_set(x, y, x + 14 / 2 - 1, y + 14 - 1);
	ch = ch - ' ';
	size = (14 / 8 + ((14 % 8) ? 1 : 0)) * (14 / 2);
	for (i = 0; i < size; i++)
	{
		temp = asc2_1407[ch][i];
		for (j = 0; j < 7; j++)
		{
			if (temp & 0x80)
			{
				tft_send_data_u16(font_color);
			}
			else
			{
				tft_send_data_u16(back_color);
			}

			temp <<= 1;
		}
	}
}
void tft_ShowStr14_14(uint16_t x, uint16_t y, uint32_t max_width, char *str, uint16_t font_color, uint16_t back_color)
{
	max_width += x;
	while ((*str <= '~') && (*str >= ' '))
	{
		if (x >= max_width)
		{
			break;
		}
		tft_ShowChar14_14(x, y, *str, font_color, back_color);
		x += 14 / 2;
		str++;
	}
}

void LCD_Image(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image)
{
    tft_address_set(x, y, x + width - 1, y + height - 1);
    
    // 一次性传输整个图像数据
    HAL_SPI_Transmit_DMA(&hspi1, (uint8_t*)image, width * height * 2);
    
    // 等待DMA传输完成
    while(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
}
void LCD_Imagevertical(uint16_t x, uint16_t y, uint16_t width, uint16_t height, const uint8_t *image)
{
    // 设置显示窗口
    tft_address_set(x, y, x + width - 1, y + height - 1);
    
    // 创建行缓冲区
    uint8_t line_buffer[width * 2];
    
    for (uint16_t row = 0; row < height; row++)
    {
        // 填充行缓冲区
        for (uint16_t col = 0; col < width; col++)
        {			
            uint32_t index = (col * height + row) * 2;
            uint32_t buffer_index = col * 2;
            
            line_buffer[buffer_index] = image[index];
            line_buffer[buffer_index + 1] = image[index + 1];
        }
        
        // 批量传输整行数据
        HAL_SPI_Transmit_DMA(&hspi1, line_buffer, width * 2);
        while(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
    }
}
/**
 * @brief 显示列优先格式图片的指定部分
 * @param x, y 屏幕上显示区域的起始坐标
 * @param src_x, src_y 图片源数据的起始坐标
 * @param width, height 要显示的矩形区域尺寸
 * @param img_width, img_height 原始图片的总尺寸
 * @param image 图片数据指针
 */
void LCD_ImageVerticalPartial(uint16_t x, uint16_t y, uint16_t src_x, uint16_t src_y, 
                             uint16_t width, uint16_t height, uint16_t img_width, 
                             uint16_t img_height, const uint8_t *image)
{
    // 参数校验和边界处理
    if (src_x >= img_width || src_y >= img_height) return;
    
    uint16_t actual_width = (src_x + width > img_width) ? (img_width - src_x) : width;
    uint16_t actual_height = (src_y + height > img_height) ? (img_height - src_y) : height;
    
    if (actual_width == 0 || actual_height == 0) return;

    // 创建行缓冲区
    uint8_t line_buffer[actual_width * 2]; // 每行像素数据
    
    tft_address_set(x, y, x + actual_width - 1, y + actual_height - 1);

    // 逐行处理
    for (uint16_t row = 0; row < actual_height; row++)
    {
        uint16_t src_row = src_y + row;
        uint32_t buffer_index = 0;
        
        // 填充行缓冲区
        for (uint16_t col = 0; col < actual_width; col++)
        {
            uint16_t src_col = src_x + col;
            // 列优先格式索引：列索引 × 图片总高度 + 行索引
            uint32_t image_index = (src_col * img_height + src_row) * 2;
            
            line_buffer[buffer_index++] = image[image_index];     // 高字节
            line_buffer[buffer_index++] = image[image_index + 1]; // 低字节
        }
        
        // 批量传输整行数据
        HAL_SPI_Transmit_DMA(&hspi1, line_buffer, actual_width * 2);
        while(HAL_SPI_GetState(&hspi1) == HAL_SPI_STATE_BUSY_TX);
    }
}

/**
 * @brief 清除指定矩形区域
 * @param x_start 区域左上角X坐标
 * @param y_start 区域左上角Y坐标
 * @param x_end 区域右下角X坐标
 * @param y_end 区域右下角Y坐标
 * @param bg_color 填充颜色（RGB565）
 */
void TFT_ClearArea(uint16_t x_start, uint16_t y_start, uint16_t x_end, uint16_t y_end, uint16_t bg_color)
{
	// 参数校验（确保不越界）
	x_end = (x_end >= TFT_COLUMN_NUMBER) ? TFT_COLUMN_NUMBER - 1 : x_end;
	y_end = (y_end >= TFT_LINE_NUMBER) ? TFT_LINE_NUMBER - 1 : y_end;

	// 设置显示区域
	tft_address_set(x_start, y_start, x_end, y_end);

	// 预计算颜色值
	uint8_t hi = bg_color >> 8;
	uint8_t lo = bg_color & 0xFF;

	// 计算像素总数
	uint32_t total_pixels = (x_end - x_start + 1) * (y_end - y_start + 1);

	// 填充颜色
	while (total_pixels--)
	{
		tft_send_data(hi);
		tft_send_data(lo);
	}
}  



void tft_power_off(void)
{
	tft_full(SKY_BLUE);
	if (sys.language == 0) // 中文
	{
		LCD_Image(5, 50, 150, 25, gImage_power_on);
	}
	else
	{
		LCD_Image(25, 40, 110, 44, gImage_power_on_en);	
	}

}
void tft_power_on(void)
{
	tft_full(SKY_BLUE);
  if (sys.ID == LED_700)
  {
		tft_ShowStr12_12(60, 3, 120, "LED 700", BLACK, SKY_BLUE);		
	  sys.green = 100; // 绿光初始值
    sys.red = 100;   // 红光初始值	
  }
	else if (sys.ID == LED_500)
  {
		tft_ShowStr12_12(60, 3, 120, "LED 500", BLACK, SKY_BLUE);		
	  sys.green = 100; // 绿光初始值
    sys.red = 100;   // 红光初始值			
  }
	else if (sys.ID == LED_6)
  {
		tft_ShowStr12_12(60, 3, 120, "LED 6", BLACK, SKY_BLUE);	
	  sys.green = 30; // 绿光初始值
    sys.red = 30;   // 红光初始值			
  }	
	else if (sys.ID == LED_4)
  {
		tft_ShowStr12_12(60, 3, 120, "LED 4", BLACK, SKY_BLUE);	
	  sys.green = 30; // 绿光初始值
    sys.red = 30;   // 红光初始值			
  }	
	tft_ShowStr12_12(132, 3, 120, "V1.0", BLACK, SKY_BLUE);
	tft_drawLine(02, 15, 158, 15, BLACK);
	if (sys.language == 0) // 中文
	{
		display_char12_12(51, 18, WHITE, SKY_BLUE, chinese_lum[0]);
		display_char12_12(63, 18, WHITE, SKY_BLUE, chinese_lum[1]);
	}
	else // 英文
	{
		tft_ShowStr14_14(51, 17, 120, "LUM", WHITE, SKY_BLUE);
	}
	tft_ShowChar14_14(75, 17, ':', WHITE, SKY_BLUE);
	LCD_Imagevertical(02, 35, 150, 5, gImage_verticallum150);
	data_100to150(sys.lum, LUM); // 显示照度数据
	if (sys.language == 0)		 // 中文
	{
		display_char12_12(51, 45, WHITE, SKY_BLUE, chinese_temp[0]);
		display_char12_12(63, 45, WHITE, SKY_BLUE, chinese_temp[1]);
	}
	else // 英文
	{
		tft_ShowStr14_14(45, 44, 120, "TEMP", WHITE, SKY_BLUE);
	}
	tft_ShowChar14_14(75, 44, ':', WHITE, SKY_BLUE);
	LCD_Imagevertical(02, 63, 150, 5, gImage_verticaltemp150);
	data_100to150(sys.temp, TEMP); // 显示色温数据

	tft_ShowStr12_12(25, 74, 120, "10", BLACK, SKY_BLUE);
	tft_ShowStr12_12(60, 74, 120, "20", BLACK, SKY_BLUE);
	tft_ShowStr12_12(95, 74, 120, "50", BLACK, SKY_BLUE);
	tft_ShowStr12_12(130, 74, 120, "100", BLACK, SKY_BLUE);
	/*	背景  */
	if (sys.language == 0) // 中文
	{
		display_char12_12(40, 93, BLACK, SKY_BLUE, chinese_chars1[0]);
		display_char12_12(52, 93, BLACK, SKY_BLUE, chinese_chars1[1]);
	}
	else
	{
		tft_ShowStr12_12(40, 93, 120, "BACK", BLACK, SKY_BLUE);
	}

	/*	深部照射  */
	if (sys.language == 0) // 中文
	{
		display_char12_12(90, 93, BLACK, SKY_BLUE, chinese_chars2[0]);
		display_char12_12(102, 93, BLACK, SKY_BLUE, chinese_chars2[1]);
		display_char12_12(114, 93, BLACK, SKY_BLUE, chinese_chars2[2]);
		display_char12_12(126, 93, BLACK, SKY_BLUE, chinese_chars2[3]);
	}
	else
	{
		tft_ShowStr12_12(90, 93, 120, "DEEP", BLACK, SKY_BLUE);
	}
	/*	浅表  */
	if (sys.language == 0) // 中文
	{
		display_char12_12(25, 110, BLACK, SKY_BLUE, chinese_chars3[0]);
		display_char12_12(37, 110, BLACK, SKY_BLUE, chinese_chars3[1]);
	}
	else
	{
		tft_ShowStr12_12(16, 110, 120, "SURF", BLACK, SKY_BLUE);
	}
	/*	深腔  */
	if (sys.language == 0) // 中文
	{
		display_char12_12(75, 110, BLACK, SKY_BLUE, chinese_chars4[0]);
		display_char12_12(87, 110, BLACK, SKY_BLUE, chinese_chars4[1]);
	}
	else
	{
		tft_ShowStr12_12(55, 110, 120, "DEEP CAV", BLACK, SKY_BLUE);
	}
	/*	正常 */
	if (sys.language == 0) // 中文
	{
		display_char12_12(125, 110, BLACK, SKY_BLUE, chinese_chars5[0]);
		display_char12_12(137, 110, BLACK, SKY_BLUE, chinese_chars5[1]);
	}
	else
	{
		tft_ShowStr12_12(120, 110, 120, "NORMAL", BLACK, SKY_BLUE);
	}
//	pwm_set(); // 设置PWM
	tft_level();
	tft_mode();
}
void tft_level(void)
{
	if (sys.level == 0)
	{
		LCD_Image(10, 74, 12, 12, gImage_select_on);
		LCD_Image(45, 74, 12, 12, gImage_select_off);
		LCD_Image(80, 74, 12, 12, gImage_select_off);
		LCD_Image(115, 74, 12, 12, gImage_select_off);
	}
	else if (sys.level == 1)
	{
		LCD_Image(10, 74, 12, 12, gImage_select_off);
		LCD_Image(45, 74, 12, 12, gImage_select_on);
		LCD_Image(80, 74, 12, 12, gImage_select_off);
		LCD_Image(115, 74, 12, 12, gImage_select_off);
	}
	else if (sys.level == 2)
	{
		LCD_Image(10, 74, 12, 12, gImage_select_off);
		LCD_Image(45, 74, 12, 12, gImage_select_off);
		LCD_Image(80, 74, 12, 12, gImage_select_on);
		LCD_Image(115, 74, 12, 12, gImage_select_off);
	}
	else if (sys.level == 3)
	{
		LCD_Image(10, 74, 12, 12, gImage_select_off);
		LCD_Image(45, 74, 12, 12, gImage_select_off);
		LCD_Image(80, 74, 12, 12, gImage_select_off);
		LCD_Image(115, 74, 12, 12, gImage_select_on);
	}
}
void tft_mode(void)
{
	if (sys.language == 0)
	{
		if (sys.mode == 0) // 背景
		{
			LCD_Image(25, 93, 12, 12, gImage_select_on);
			LCD_Image(75, 93, 12, 12, gImage_select_off);
			LCD_Image(10, 110, 12, 12, gImage_select_off);
			LCD_Image(60, 110, 12, 12, gImage_select_off);
			LCD_Image(110, 110, 12, 12, gImage_select_off);
		}
		else if (sys.mode == 1) // 深部照射
		{
			LCD_Image(25, 93, 12, 12, gImage_select_off);
			LCD_Image(75, 93, 12, 12, gImage_select_on);
			LCD_Image(10, 110, 12, 12, gImage_select_off);
			LCD_Image(60, 110, 12, 12, gImage_select_off);
			LCD_Image(110, 110, 12, 12, gImage_select_off);
		}
		else if (sys.mode == 2) // 浅表
		{
			LCD_Image(25, 93, 12, 12, gImage_select_off);
			LCD_Image(75, 93, 12, 12, gImage_select_off);
			LCD_Image(10, 110, 12, 12, gImage_select_on);
			LCD_Image(60, 110, 12, 12, gImage_select_off);
			LCD_Image(110, 110, 12, 12, gImage_select_off);
		}
		else if (sys.mode == 3) // 深腔
		{
			LCD_Image(25, 93, 12, 12, gImage_select_off);
			LCD_Image(75, 93, 12, 12, gImage_select_off);
			LCD_Image(10, 110, 12, 12, gImage_select_off);
			LCD_Image(60, 110, 12, 12, gImage_select_on);
			LCD_Image(110, 110, 12, 12, gImage_select_off);
		}
		else if (sys.mode == 4) // 正常
		{
			LCD_Image(25, 93, 12, 12, gImage_select_off);
			LCD_Image(75, 93, 12, 12, gImage_select_off);
			LCD_Image(10, 110, 12, 12, gImage_select_off);
			LCD_Image(60, 110, 12, 12, gImage_select_off);
			LCD_Image(110, 110, 12, 12, gImage_select_on);
		}
	}
	else
	{
		if (sys.mode == 0) // 背景
		{
			LCD_Image(25, 93, 12, 12, gImage_select_on);
			LCD_Image(75, 93, 12, 12, gImage_select_off);
			LCD_Image(02, 110, 12, 12, gImage_select_off);
			LCD_Image(42, 110, 12, 12, gImage_select_off);
			LCD_Image(105, 110, 12, 12, gImage_select_off);
		}
		else if (sys.mode == 1) // 深部照射
		{
			LCD_Image(25, 93, 12, 12, gImage_select_off);
			LCD_Image(75, 93, 12, 12, gImage_select_on);
			LCD_Image(02, 110, 12, 12, gImage_select_off);
			LCD_Image(42, 110, 12, 12, gImage_select_off);
			LCD_Image(105, 110, 12, 12, gImage_select_off);
		}
		else if (sys.mode == 2) // 浅表
		{
			LCD_Image(25, 93, 12, 12, gImage_select_off);
			LCD_Image(75, 93, 12, 12, gImage_select_off);
			LCD_Image(02, 110, 12, 12, gImage_select_on);
			LCD_Image(42, 110, 12, 12, gImage_select_off);
			LCD_Image(105, 110, 12, 12, gImage_select_off);
		}
		else if (sys.mode == 3) // 深腔
		{
			LCD_Image(25, 93, 12, 12, gImage_select_off);
			LCD_Image(75, 93, 12, 12, gImage_select_off);
			LCD_Image(02, 110, 12, 12, gImage_select_off);
			LCD_Image(42, 110, 12, 12, gImage_select_on);
			LCD_Image(105, 110, 12, 12, gImage_select_off);
		}
		else if (sys.mode == 4) // 正常
		{
			LCD_Image(25, 93, 12, 12, gImage_select_off);
			LCD_Image(75, 93, 12, 12, gImage_select_off);
			LCD_Image(02, 110, 12, 12, gImage_select_off);
			LCD_Image(42, 110, 12, 12, gImage_select_off);
			LCD_Image(105, 110, 12, 12, gImage_select_on);
		}
	}
}
extern uint8_t key_add_sum_flag;
uint8_t old_lum, old_temp;
uint8_t sum_show;
void data_100to150(uint8_t data, uint8_t type) // 实际1~100%的数据转换为0~150的进度条
{

	uint8_t odd_count = (data + 1) / 2; // 奇数个数
	uint8_t even_count = data / 2;		// 偶数个数
	int sum = odd_count + 2 * even_count;
	sum_show = sum;
	char buff[10];
	// sprintf(buff, "%-3d%%", data);
	sprintf(buff, "%d", data);
	if (data >= 100)
	{
		strcat(buff, "%");
	}
	else if (data >= 10)
	{
		strcat(buff, "%");
		strcat(buff, " ");
	}
	else
	{
		buff[2] = '\0';
		buff[1] = '\0';
		strcat(buff, "%");
		strcat(buff, " ");
		strcat(buff, " ");
	}
	if (type == LUM) // 照度
	{
		tft_ShowStr14_14(88, 17, 120, buff, WHITE, SKY_BLUE);
		// 		TFT_ClearArea(03, 32, 156, 43, SKY_BLUE);
		//		LCD_Imagevertical(03, 35, 70, 5, gImage_verticallum150);
		if (old_lum != 0)
		{
			TFT_ClearArea(old_lum + 02, 32, old_lum + 02 + 05, 43, SKY_BLUE);
			LCD_ImageVerticalPartial(old_lum + 02, 35, // 屏幕上的起始坐标
									 old_lum, 0,	   // 图片源数据的起始坐标
									 05 + 01, 5,	   // 要显示的矩形区域尺寸
									 150, 5,		   // 原始图片的尺寸
									 gImage_verticallum150);
		}
		LCD_Image(sum + 02, 32, 4, 11, gImage_bjx);
		old_lum = sum;
	}
	else if (type == TEMP) // 色温
	{
		tft_ShowStr14_14(88, 44, 120, buff, WHITE, SKY_BLUE);
		//		TFT_ClearArea(03, 60, 156, 73, SKY_BLUE);
		//		LCD_Image(03, 63, 150, 5, gImage_temp150);
		if (old_temp != 0)
		{
			TFT_ClearArea(old_temp + 02, 60, old_temp + 02 + 05, 73, SKY_BLUE);
			LCD_ImageVerticalPartial(old_temp + 02, 63, // 屏幕上的起始坐标
									 old_temp, 0,		// 图片源数据的起始坐标
									 05 + 01, 5,		// 要显示的矩形区域尺寸
									 150, 5,			// 原始图片的尺寸
									 gImage_verticaltemp150);
		}
		LCD_Image(sum + 02, 60, 4, 11, gImage_bjx);
		old_temp = sum;
	}

	// todo 闪烁问题已解决
}
