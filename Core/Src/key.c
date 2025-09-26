#include "key.h"
#include "tft.h"
#include "usart_control.h"
#include "pwm_control.h"
#include "myflash.h"
SystemDef sys;
uint8_t KeyScan(void)
{
  static uint8_t key1_state = 0, key2_state = 0, key3_state = 0, key4_state = 0, key5_state = 0, key6_state = 0, key7_state = 0, key8_state = 0, key9_state = 0,key10_state = 0;
  static uint16_t press_time = 0;
  static uint16_t press_time_long = 0;
  uint8_t KeyTemp = 0;
  if (HAL_GPIO_ReadPin(LUM_SUB_GPIO_Port, LUM_SUB_Pin) == 0) // 检测到按键按下
  {
    if (key1_state == 0)
    {
      key1_state = 1;
      press_time = 0;
      press_time_long = 0;
    }
    press_time++;
    if (press_time >= LONG_PRESS_TIME) // 把长按添加这里的原因是 长按后可不用松开就执行长按程序
    {
      press_time_long++;
      if (press_time_long == LONG_PRESS_TIME_2)
      {
        press_time_long = 0;
        KeyTemp = KEY1 | KEY_LONG;
      }
      else
        KeyTemp = 0;
    }
  }
  else // 按键松开
  {
    if (key1_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY1 | KEY_SHORT;
      }
      key1_state = 0;
    }
  }

  if (HAL_GPIO_ReadPin(LUM_ADD_GPIO_Port, LUM_ADD_Pin) == 0) // 检测到按键按下
  {
    if (key2_state == 0)
    {
      key2_state = 1;
      press_time = 0;
      press_time_long = 0;
    }
    press_time++;
    if (press_time >= LONG_PRESS_TIME) // 把长按添加这里的原因是 长按后可不用松开就执行长按程序
    {
      press_time_long++;
      if (press_time_long == LONG_PRESS_TIME_2)
      {
        press_time_long = 0;
        KeyTemp = KEY2 | KEY_LONG;
      }
      else
        KeyTemp = 0;
    }
  }
  else // 按键松开
  {
    if (key2_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY2 | KEY_SHORT;
      }
      key2_state = 0;
    }
  }

  if (HAL_GPIO_ReadPin(TEMP_SUB_GPIO_Port, TEMP_SUB_Pin) == 0) // 检测到按键按下
  {
    if (key3_state == 0)
    {
      key3_state = 1;
      press_time = 0;
      press_time_long = 0;
    }
    press_time++;
    if (press_time >= LONG_PRESS_TIME) // 把长按添加这里的原因是 长按后可不用松开就执行长按程序
    {
      press_time_long++;
      if (press_time_long == LONG_PRESS_TIME_2)
      {
        press_time_long = 0;
        KeyTemp = KEY3 | KEY_LONG;
      }
      else
        KeyTemp = 0;
    }
  }
  else // 按键松开
  {
    if (key3_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY3 | KEY_SHORT;
      }
      key3_state = 0;
    }
  }

  if (HAL_GPIO_ReadPin(TEMP_ADD_GPIO_Port, TEMP_ADD_Pin) == 0) // 检测到按键按下
  {
    if (key4_state == 0)
    {
      key4_state = 1;
      press_time = 0;
      press_time_long = 0;
    }
    press_time++;
    if (press_time >= LONG_PRESS_TIME) // 把长按添加这里的原因是 长按后可不用松开就执行长按程序
    {
      press_time_long++;
      if (press_time_long == LONG_PRESS_TIME_2)
      {
        press_time_long = 0;
        KeyTemp = KEY4 | KEY_LONG;
      }
      else
        KeyTemp = 0;
    }
  }
  else // 按键松开
  {
    if (key4_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY4 | KEY_SHORT;
      }
      key4_state = 0;
    }
  }

  if (HAL_GPIO_ReadPin(R9_GPIO_Port, R9_Pin) == 0) // 检测到按键按下
  {
    if (key5_state == 0)
    {
      key5_state = 1;
      press_time = 0;
    }
    press_time++;
  }
  else // 按键松开
  {
    if (key5_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY5 | KEY_SHORT;
      }
      key5_state = 0;
    }
  }

  if (HAL_GPIO_ReadPin(R11_GPIO_Port, R11_Pin) == 0) // 检测到按键按下
  {
    if (key6_state == 0)
    {
      key6_state = 1;
      press_time = 0;
    }
    press_time++;
  }
  else // 按键松开
  {
    if (key6_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY6 | KEY_SHORT;
      }
      key6_state = 0;
    }
  }

  if (HAL_GPIO_ReadPin(ON_OFF_GPIO_Port, ON_OFF_Pin) == 0) // 检测到按键按下
  {
    if (key7_state == 0)
    {
      key7_state = 1;
      press_time = 0;
    }
    press_time++;
  }
  else // 按键松开
  {
    if (key7_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY7 | KEY_SHORT;
      }
      key7_state = 0;
    }
  }

  if (HAL_GPIO_ReadPin(MODE_GPIO_Port, MODE_Pin) == 0) // 检测到按键按下
  {
    if (key8_state == 0)
    {
      key8_state = 1;
      press_time = 0;
    }
    press_time++;
  }
  else // 按键松开
  {
    if (key8_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY8 | KEY_SHORT;
      }
      key8_state = 0;
    }
  }

  if (HAL_GPIO_ReadPin(LEVEL_GPIO_Port, LEVEL_Pin) == 0) // 检测到按键按下
  {
    if (key9_state == 0)
    {
      key9_state = 1;
      press_time = 0;
    }
    press_time++;
  }
  else // 按键松开
  {
    if (key9_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY9 | KEY_SHORT;
      }
      key9_state = 0;
    }
  }
  if (HAL_GPIO_ReadPin(LANGUAGE_GPIO_Port, LANGUAGE_Pin) == 0) // 检测到按键按下
  {
    if (key10_state == 0)
    {
      key10_state = 1;
      press_time = 0;
    }
    press_time++;
  }
  else // 按键松开
  {
    if (key10_state == 1)
    {
      if (press_time > SHORT_PRESS_TIME) // 短按
      {
        KeyTemp = KEY10 | KEY_SHORT;
      }
      key10_state = 0;
    }
  }
  return KeyTemp;
}
uint8_t level;
void Param_init(void)
{
  sys.lum = 50;     // 照度初始值
  sys.temp = 50;    // 色温初始值
//  sys.green = 100; // 绿光初始值
//  sys.red = 100;   // 红光初始值	  在tft_power_on 中对700 500  6 4 做区分
  sys.green_sw = 0; // 绿光开关初始值
  sys.red_sw = 0;   // 红光开关初始值
  sys.on_off = 0;   // 开关机初始值
  sys.level = 3;    // 档位初始值
  sys.mode = 4;     // 模式初始值
  level = 1;
  sys.language = sys.language_flash; //语言 0中文 1英文
	sys.pwm_change_mode = PWM_CHANGE_SLOW;
	sys.pwm_change_time = pwm_change_time_default;
}
uint8_t key_return;
uint8_t key_add_sum_flag;
void KeyFunc(void)
{
  uint8_t key;

  key = KeyScan();
  if (key != KEY_NULL)
  {
    switch (key & 0xF0)
    {
    case KEY1:             // 照度-
      if (sys.on_off == 0) // 只有开机状态下才允许调整
        break;
      sys.lum = sys.lum - level;
      if (sys.lum <= level)
        sys.lum = level;
      key_add_sum_flag = 1;
      data_100to150(sys.lum, LUM); // 显示照度数据
      break;
    case KEY2:             // 照度+
      if (sys.on_off == 0) // 只有开机状态下才允许调整
        break;
      sys.lum = sys.lum + level;
      if (sys.lum >= 100)
        sys.lum = 100;
      key_add_sum_flag = 2;
      data_100to150(sys.lum, LUM); // 显示照度数据
      break;
    case KEY3:             // 色温-
      if (sys.on_off == 0) // 只有开机状态下才允许调整
        break;
      sys.temp = sys.temp - level;
      if (sys.temp <= level)
        sys.temp = level;
			key_add_sum_flag = 1;
      data_100to150(sys.temp, TEMP); // 显示色温数据
      break;
    case KEY4:             // 色温+
      if (sys.on_off == 0) // 只有开机状态下才允许调整
        break;
      sys.temp = sys.temp + level;
      if (sys.temp >= 100)
        sys.temp = 100;
			key_add_sum_flag = 2;
      data_100to150(sys.temp, TEMP); // 显示色温数据
      break;
    case KEY5:             // R9红光
      if (sys.on_off == 0) // 只有开机状态下才允许调整
        break;
      sys.red_sw = !sys.red_sw;
      break;
    case KEY6:             // R11绿光
      if (sys.on_off == 0) // 只有开机状态下才允许调整
        break;
      sys.green_sw = !sys.green_sw;
      break;
    case KEY7: // 开关机
      sys.on_off = !sys.on_off;
      if (sys.on_off == 0) // 关机
      {
        tft_power_off();
      }
      else // 开机
      {
        tft_power_on(); // 显示界面
      }
      break;
    case KEY9:             // 档位 0-3循环
      if (sys.on_off == 0) // 只有开机状态下才允许调整
        break;
      sys.level = (sys.level + 1) % 4;
      tft_level();
      level_set(); // 更新档位
      break;
    case KEY8:             // 模式 0-4循环
      if (sys.on_off == 0) // 只有开机状态下才允许调整
        break;
      sys.mode = (sys.mode + 1) % 5;
      tft_mode();
      //pwm_set(); // 更新PWM值
      break;
    case KEY10:             // 语言
			sys.language = !sys.language;
      sys.language_flash = sys.language;
      WriteInf();
      if (sys.on_off == 0) // 关机
      {
        tft_power_off();
      }
      else // 开机
      {
        tft_power_on(); // 显示界面
      }
      break;			
			
    default:
      break;
    }
  }
}
void level_set(void) // 设置档位
{
  uint16_t lum, temp;
	uint16_t lum_old, temp_old;
  if (sys.level == 0) // 10
  {
    level = 10;
    if (sys.lum < 10)
      sys.lum = 10; // 确保照度不低于10
    if (sys.temp < 10)
      sys.temp = 10; // 确保色温不低于10
  }
  else if (sys.level == 1) // 20
  {
    level = 5;
    if (sys.lum < 5)
      sys.lum = 5; // 确保照度不低于5
    if (sys.temp < 5)
      sys.temp = 5; // 确保色温不低于5
  }
  else if (sys.level == 2) // 50
  {
    level = 2;
    if (sys.lum < 2)
      sys.lum = 2; // 确保照度不低于2
    if (sys.temp < 2)
      sys.temp = 2; // 确保色温不低于2
  }
  else if (sys.level == 3) // 100
  {
    level = 1;
    if (sys.lum < 1)
      sys.lum = 1; // 确保照度不低于1
    if (sys.temp < 1)
      sys.temp = 1; // 确保色温不低于1
  }
  lum = (sys.lum - level) / level;
  temp = (sys.temp - level) / level;
	
	lum_old = sys.lum;
	temp_old = sys.temp;
  sys.lum = level + lum * level;
  sys.temp = level + temp * level;
	if(sys.lum != lum_old)
		data_100to150(sys.lum, LUM);   // 显示照度数据
	if(sys.temp != temp_old)	
		data_100to150(sys.temp, TEMP); // 显示色温数据
}
