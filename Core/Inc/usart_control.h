#ifndef __USART_CONTROL_H__
#define __USART_CONTROL_H__
#include "gpio.h"
#define CMD_HEAD 0XEE       // 帧头
#define CMD_TAIL 0XFFFCFFFF // 帧尾
#define CMD_MAX_SIZE 64     // 单条指令大小，根据需要调整，尽量设置大一些
#define QUEUE_MAX_SIZE 64   // 指令接收缓冲区大小，根据需要调整，尽量设置大一些
#define PTR2U16(PTR) ((((uint8_t *)(PTR))[0] << 8) | ((uint8_t *)(PTR))[1])
#define PTR2U32(PTR) ((((uint8_t *)(PTR))[0]<<24)|(((uint8_t *)(PTR))[1]<<16)|(((uint8_t *)(PTR))[2]<<8)|((uint8_t *)(PTR))[3])  //从缓冲区取32位数据

#define LED_700 1
#define LED_500 2
#define LED_6 3
#define LED_4 4


#define LED_700_LUM_MIN 485
#define LED_700_LUM_MAX 1420

#define LED_700_TEMP_3000_MAX 780 // 暖光	1%
#define LED_700_TEMP_3000_MIN 295 // 暖光 100%

#define LED_700_TEMP_5000_MIN 100 // 冷光 	1%
#define LED_700_TEMP_5000_MAX 780 // 冷光 100%



typedef unsigned char qdata;
typedef unsigned short qsize;
#pragma pack(push)
#pragma pack(1)                           //按字节对齐
typedef struct
{
    uint8_t cmd_head;    // EE            帧头
    uint8_t cmd_type;    // 00            功能码
    uint32_t data;       // 00 00 00 00   数据
    uint16_t crc;        // 11 11         crc(计算功能码和数据)
    uint8_t cmd_tail[4]; // FF FC FF FF   帧尾
} CTRL_MSG, *PCTRL_MSG;
#pragma pack(pop)
typedef struct _QUEUE
{
    qsize _head;     // 队列头
    qsize _tail;     // 队列尾
    qdata _data[50]; // 队列数据缓存区
} QUEUE;
//void pwm_set(void);
//void pwm_change(void);
void JudgmentMessage(void);
void huart1_send(uint8_t cmd_type, uint32_t data);
void AddCRC16(uint8_t *buffer, uint16_t n, uint16_t *pcrc);
uint16_t CheckCRC16(uint8_t *buffer, uint16_t n);
void queue_push(qdata _data);
qsize queue_find_cmd(qdata *buffer, qsize buf_len);
void ProcessMessage(PCTRL_MSG msg, uint16_t size);
#endif
