#ifndef __KEY_H__
#define __KEY_H__
#include "gpio.h"
#include "stdint.h"
#include <stdlib.h>
#include <stdio.h>
// 定义按键值
#define KEY_NULL 0X0F
#define KEY1 0x10               // 照度-
#define KEY2 0x20               // 照度+
#define KEY3 0x30               // 色温-
#define KEY4 0x40               // 色温+
#define KEY5 0x50               // 绿光
#define KEY6 0x60               // 红光
#define KEY7 0x70               // 开关机
#define KEY8 0x80               // 档位
#define KEY9 0x90               // 模式
#define KEY10 0xa0              // 语言
#define SHORT_PRESS_TIME 50     // 短按时间阈值，单位：ms
#define LONG_PRESS_TIME 400     // 长按时间阈值，单位：ms
#define LONG_PRESS_TIME_2s 2000 // 长按时间阈值，单位：ms
#define LONG_PRESS_TIME_2 80    // 长按时间阈值，单位：ms
#define KEY_SHORT 0x01
#define KEY_LONG 0x02

typedef struct
{
    uint16_t lum;            // 照度
    uint16_t temp;           // 色温
    uint16_t green;          // 绿光
    uint16_t red;            // 红光
    uint16_t green_sw;       // 绿光开关
    uint16_t red_sw;         // 红光开关
    uint16_t on_off;         // 开关机
    uint16_t level;          // 档位
    uint16_t mode;           // 模式
    uint16_t ID;             // ID
    uint16_t language;       // 语言
    uint16_t language_flash; // 语言flash 长按3秒中英文切换键，更换开机显示
    uint16_t pwm_change_mode;
    uint32_t pwm_change_time;

} SystemDef;
void Param_init(void);
void KeyFunc(void);
void level_set(void);
#endif
