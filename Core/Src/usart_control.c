#include "usart_control.h"
#include "usart.h"
#include "key.h"
#include "tim.h"
extern uint8_t data1;
extern SystemDef sys;
uint8_t UART1_RxBuf[CMD_MAX_SIZE]; // 指令缓存
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart == &huart1)
    {
        queue_push(data1);
        HAL_UART_Receive_IT(&huart1, &data1, 1);
    }
}
void JudgmentMessage(void)
{
    uint8_t size = 0; // 接收到的指令长度
    size = queue_find_cmd(UART1_RxBuf, CMD_MAX_SIZE);
    if (size > 0) // 接收到指令
    {
        ProcessMessage((PCTRL_MSG)UART1_RxBuf, size); // 指令处理
    }
}
void ProcessMessage(PCTRL_MSG msg, uint16_t size)
{	  
    uint8_t cmd_type = msg->cmd_type;    // 功能码
    uint32_t data = PTR2U32(&msg->data); // 数据
    switch (cmd_type)
    {
    case 0x01:               // 握手
        sys.ID = data >> 16; // LED700
        break;
    default:
        break;
    }
}
uint8_t frame[12] = {
    0xEE, // 帧头
    0x00, // 指令码
    0x00, // 指令内容
    0x00, // 指令内容
    0x00, // 指令内容
    0x00, // 指令内容
    0x00, // CRC16低字节
    0x00, // CRC16高字节
    0xFF, // 帧尾
    0xFC, // 帧尾
    0xFF, // 帧尾
    0xFF  // 帧尾
};
uint16_t Calculate_CRC16(const uint8_t *data, uint16_t length)
{
    uint16_t crc = 0xFFFF;
    for (uint16_t i = 0; i < length; i++)
    {
        crc ^= data[i];
        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 0x0001)
            {
                crc = (crc >> 1) ^ 0xA001;
            }
            else
            {
                crc = crc >> 1;
            }
        }
    }
    return crc;
}
void huart1_send(uint8_t cmd_type, uint32_t data)
{
    uint16_t crc = 0xFFFF;
    frame[1] = cmd_type;            // 指令码
    frame[2] = (data >> 24) & 0xFF; // 指令内容
    frame[3] = (data >> 16) & 0xFF; // 指令内容
    frame[4] = (data >> 8) & 0xFF;  // 指令内容
    frame[5] = (data >> 0) & 0xFF;  // 指令内容
    AddCRC16(&frame[1], 5, &crc);
    frame[6] = (crc >> 8) & 0xFF; // CRC16高字节
    frame[7] = crc & 0xFF;        // CRC16低字节
    HAL_UART_Transmit(&huart1, frame, 12, 1000);
}
uint16_t pwm_3000 = 0; // 3000K色温PWM值
uint16_t pwm_5000 = 0; // 5000K色温PWM



//!!!        串口接收相关函数        !!!//
static QUEUE que = {0, 0, 0};  // 指令队列
static uint32_t cmd_state = 0; // 队列帧尾检测状态
static qsize cmd_pos = 0;      // 当前指令指针位置
void AddCRC16(uint8_t *buffer, uint16_t n, uint16_t *pcrc)
{
    uint16_t i, j, carry_flag, a;

    for (i = 0; i < n; i++)
    {
        *pcrc = *pcrc ^ buffer[i];
        for (j = 0; j < 8; j++)
        {
            a = *pcrc;
            carry_flag = a & 0x0001;
            *pcrc = *pcrc >> 1;
            if (carry_flag == 1)
                *pcrc = *pcrc ^ 0xa001;
        }
    }
}
uint16_t CheckCRC16(uint8_t *buffer, uint16_t n)
{
    uint16_t crc0 = 0x0;
    uint16_t crc1 = 0xffff;

    if (n >= 2)
    {
        crc0 = ((buffer[n - 2] << 8) | buffer[n - 1]);
        AddCRC16(buffer, n - 2, &crc1);
    }

    return (crc0 == crc1);
}
void queue_reset(void) // 清空指令数据
{
    que._head = que._tail = 0;
    cmd_pos = cmd_state = 0;
}
void queue_push(qdata _data) // 添加指令数据
{
    qsize pos = (que._head + 1) % QUEUE_MAX_SIZE;
    if (pos != que._tail) // 非满状态
    {
        que._data[que._head] = _data;
        que._head = pos;
    }
}
void queue_pop(qdata *_data) // 从队列中取一个数据
{
    if (que._tail != que._head) // 非空状态
    {
        *_data = que._data[que._tail];
        que._tail = (que._tail + 1) % QUEUE_MAX_SIZE;
    }
}
qsize queue_size(void) // 获取队列中有效数据个数
{
    return ((que._head + QUEUE_MAX_SIZE - que._tail) % QUEUE_MAX_SIZE);
}
qsize queue_find_cmd(qdata *buffer, qsize buf_len) // 从指令队列中取出一条完整的指令
{
    qsize cmd_size = 0;
    qdata _data = 0;

    while (queue_size() > 0)
    {
        // 取一个数据
        queue_pop(&_data);

        if (cmd_pos == 0 && _data != CMD_HEAD) // 指令第一个字节必须是帧头，否则跳过
        {
            continue;
        }
        if (cmd_pos < buf_len) // 防止缓冲区溢出
            buffer[cmd_pos++] = _data;

        cmd_state = ((cmd_state << 8) | _data); // 拼接最后4个字节，组成一个32位整数

        // 最后4个字节与帧尾匹配，得到完整帧
        if (cmd_state == CMD_TAIL)
        {
            cmd_size = cmd_pos; // 指令字节长度
            cmd_state = 0;      // 重新检测帧尾巴
            cmd_pos = 0;        // 复位指令指针

            // 去掉指令头尾EE，尾FFFCFFFF共计5个字节，只计算数据部分CRC
            if (!CheckCRC16(buffer + 1, cmd_size - 5)) // CRC校验
                return 0;

            cmd_size -= 2; // 去掉CRC16（2字节）
            return cmd_size;
        }
    }
    return 0; // 未找到完整指令
}
