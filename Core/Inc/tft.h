#ifndef TFT_H_
#define TFT_H_
#include "stdint.h"
#define tft_rst_0() HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_RESET)
#define tft_rst_1() HAL_GPIO_WritePin(TFT_RST_GPIO_Port, TFT_RST_Pin, GPIO_PIN_SET)

#define tft_dc_0() HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_RESET)
#define tft_dc_1() HAL_GPIO_WritePin(TFT_DC_GPIO_Port, TFT_DC_Pin, GPIO_PIN_SET)

//#define tft_cs_0() HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_RESET)
//#define tft_cs_1() HAL_GPIO_WritePin(TFT_CS_GPIO_Port, TFT_CS_Pin, GPIO_PIN_SET)

#define BULE 0x001F        // 纯蓝
#define DEEP_BLUE 0x01CF   // 深蓝
#define SKY_BLUE 0x867F    // 天蓝
#define LIGHT_GREEN 0x07E0 // 浅绿
#define PINK 0xFC1F        // 粉色
#define RED 0xF800         // 红色
#define GREEN 0x07E0       // 绿色
#define YELLOW 0xFFE0      // 黄色
#define BLACK 0x0000       // 黑色
#define WHITE 0xFFFF       // 白色

void tft_init(void);
void tft_full(unsigned long color);
void display_char16_16(unsigned int x, unsigned int y, unsigned long fg_color, unsigned long bg_color, const unsigned char *point);
void tft_address_set(uint8_t x_start, uint8_t y_start, uint8_t x_end, uint8_t y_end);
void tft_power_off(void);
void tft_power_on(void);
void tft_level(void);
void tft_mode(void);
void tft_ShowStr14_14(uint16_t x, uint16_t y, uint32_t max_width, char *str, uint16_t font_color, uint16_t back_color);


#define LUM  0       // 照度
#define TEMP 1       // 色温
void data_100to150(uint8_t data,uint8_t type);
#endif /* TFT_H_ */
