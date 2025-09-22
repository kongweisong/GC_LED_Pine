#include "pwm_control.h"
#include "usart.h"
#include "key.h"
#include "tim.h"
#include "usart_control.h"
#include <stdbool.h>
extern SystemDef sys;	

 uint16_t pwm_outside_cold_now;		//目前外圈冷光PWM值
 uint16_t pwm_outside_warm_now;		//目前外圈暖光PWM值
 uint16_t pwm_inside_cold_now;		//目前内圈冷光PWM值
 uint16_t pwm_inside_warm_now;		//目前内圈暖光PWM值
 uint16_t pwm_red_now;		        //目前红光PWM值
 uint16_t pwm_green_now;			    //目前绿光PWM值
void pwm_send_to_driverboard(void)
{
	static uint16_t pwm_outside_cold_want;				  // 外圈冷光PWM目标值
	static uint16_t pwm_outside_warm_want;				  // 外圈暖光PWM目标值
	static uint16_t pwm_inside_cold_want;				    // 内圈冷光PWM目标值
	static uint16_t pwm_inside_warm_want;				    // 内圈暖光PWM目标值
	static uint16_t pwm_red_want;						        // 红光PWM目标值
	static uint16_t pwm_green_want;						      // 绿光PWM目标值
	uint16_t color_temp_up;								          // 色温上点的计算值（用于插值计算）
	uint16_t color_temp_down;							          // 色温下点的计算值（用于插值计算）
	
	static uint16_t pwm_outside_cold_want_calculate;// 外圈冷光PWM目标值的中间计算变量
	static uint16_t pwm_outside_cold_change_temp;		// 外圈冷光PWM变化过渡的临时变量
	static uint16_t pwm_outside_warm_want_calculate;// 外圈暖光PWM目标值的中间计算变量
	static uint16_t pwm_outside_warm_change_temp;		// 外圈暖光PWM变化过渡的临时变量
	static uint16_t pwm_inside_cold_want_calculate;	// 内圈冷光PWM目标值的中间计算变量
	static uint16_t pwm_inside_warm_want_calculate;	// 内圈暖光PWM目标值的中间计算变量
	static uint16_t pwm_inside_cold_change_temp;		// 内圈冷光PWM变化过渡的临时变量
	static uint16_t pwm_inside_warm_change_temp;		// 内圈暖光PWM变化过渡的临时变量
	
	static uint16_t pwm_red_want_calculate;				  // 红光PWM目标值的中间计算变量
	static uint16_t pwm_red_change_temp;				    // 红光PWM变化过渡的临时变量
	static uint16_t pwm_green_want_calculate;			  // 绿光PWM目标值的中间计算变量
	static uint16_t pwm_green_change_temp;				  // 绿光PWM变化过渡的临时变量
	if(sys.on_off == true)	//开机处理
	{
		switch(sys.ID)
		{
			case LED_700:		//内圈只有冷光
				switch(sys.mode)	
				{
					case NORMAL_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_700_LU_default +
										(outside_cold_pwm_700_RU_default - outside_cold_pwm_700_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_700_LD_default +
										(outside_cold_pwm_700_RD_default - outside_cold_pwm_700_LD_default)/99.0*(sys.temp-1);
						//目标外圈冷光PWM值
						pwm_outside_cold_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);

						color_temp_up   = 0.5 + outside_warm_pwm_700_LU_default -
										(outside_warm_pwm_700_LU_default - outside_warm_pwm_700_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_700_LD_default -
										(outside_warm_pwm_700_LD_default - outside_warm_pwm_700_RD_default)/99.0*(sys.temp-1);
						
						//目标外圈暖光PWM值
						pwm_outside_warm_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);
						       
						//目标内圈冷光PWM值
						pwm_inside_cold_want = pwm_outside_cold_want;	
						break;
					}
					case DEEP_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_700_LU_default +
										(outside_cold_pwm_700_RU_default - outside_cold_pwm_700_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_700_LD_default +
										(outside_cold_pwm_700_RD_default - outside_cold_pwm_700_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 1.2 * (0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1));
						if(pwm_outside_cold_want > 1000)
						{
							pwm_outside_cold_want = 1000;
						}
					
						color_temp_up   = 0.5 + outside_warm_pwm_700_LU_default -
										(outside_warm_pwm_700_LU_default - outside_warm_pwm_700_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_700_LD_default -
										(outside_warm_pwm_700_LD_default - outside_warm_pwm_700_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 1.2 * (0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1));
						if(pwm_outside_warm_want > 1000)
						{
							pwm_outside_warm_want = 1000;
						}
						
						pwm_inside_cold_want = pwm_outside_cold_want;	
						break;
					}
					case BACK_MODE:
					{
						pwm_outside_cold_want = 0;
						pwm_outside_warm_want = 0;
						pwm_inside_cold_want = 1000;
						break;
					}
					case SURFACE_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_700_LU_default +
										(outside_cold_pwm_700_RU_default - outside_cold_pwm_700_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_700_LD_default +
										(outside_cold_pwm_700_RD_default - outside_cold_pwm_700_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);

						color_temp_up   = 0.5 + outside_warm_pwm_700_LU_default -
										(outside_warm_pwm_700_LU_default - outside_warm_pwm_700_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_700_LD_default -
										(outside_warm_pwm_700_LD_default - outside_warm_pwm_700_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);
						pwm_inside_cold_want = 0;
						break;
					}
					case DEEP_IRRADIATION_MODE:
					{
						pwm_outside_cold_want = DEEP_IRRADIATION_MODE_PWM_OUTSIDE_COLD_DEFAULT;
						pwm_outside_warm_want = DEEP_IRRADIATION_MODE_PWM_OUTSIDE_WARM_DEFAULT;
						pwm_inside_cold_want = DEEP_IRRADIATION_MODE_PWM_INSIDE_COLD_DEFAULT;
						pwm_inside_warm_want = DEEP_IRRADIATION_MODE_PWM_INSIDE_WARM_DEFAULT;
						break;
					}
					default:
						break;
				}
				break;
			case LED_500:
				switch(sys.mode)	
				{
					case NORMAL_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_500_LU_default +
										(outside_cold_pwm_500_RU_default - outside_cold_pwm_500_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_500_LD_default +
										(outside_cold_pwm_500_RD_default - outside_cold_pwm_500_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);

						color_temp_up   = 0.5 + outside_warm_pwm_500_LU_default -
										(outside_warm_pwm_500_LU_default - outside_warm_pwm_500_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_500_LD_default -
										(outside_warm_pwm_500_LD_default - outside_warm_pwm_500_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);
						pwm_inside_cold_want = pwm_outside_cold_want;
						break;
					}
					case DEEP_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_500_LU_default +
										(outside_cold_pwm_500_RU_default - outside_cold_pwm_500_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_500_LD_default +
										(outside_cold_pwm_500_RD_default - outside_cold_pwm_500_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 1.2 * (0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1));
						if(pwm_outside_cold_want > 1000)
						{
							pwm_outside_cold_want = 1000;
						}
					
						color_temp_up   = 0.5 + outside_warm_pwm_500_LU_default -
										(outside_warm_pwm_500_LU_default - outside_warm_pwm_500_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_500_LD_default -
										(outside_warm_pwm_500_LD_default - outside_warm_pwm_500_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 1.2 * (0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1));
						if(pwm_outside_cold_want > 1000)
						{
							pwm_outside_cold_want = 1000;
						}
						
						pwm_inside_cold_want = pwm_outside_cold_want;	
						break;
					}
					case BACK_MODE:
					{
						pwm_outside_cold_want = 0;
						pwm_outside_warm_want = 0;
						pwm_inside_cold_want = 1000;
						break;
					}
					case SURFACE_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_500_LU_default +
										(outside_cold_pwm_500_RU_default - outside_cold_pwm_500_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_500_LD_default +
										(outside_cold_pwm_500_RD_default - outside_cold_pwm_500_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);

						color_temp_up   = 0.5 + outside_warm_pwm_500_LU_default -
										(outside_warm_pwm_500_LU_default - outside_warm_pwm_500_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_500_LD_default -
										(outside_warm_pwm_500_LD_default - outside_warm_pwm_500_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);
						pwm_inside_cold_want = 0;
						break;
					}
					case DEEP_IRRADIATION_MODE:
					{
						pwm_outside_cold_want = DEEP_IRRADIATION_MODE_PWM_OUTSIDE_COLD_DEFAULT;
						pwm_outside_warm_want = DEEP_IRRADIATION_MODE_PWM_OUTSIDE_WARM_DEFAULT;
						pwm_inside_cold_want = DEEP_IRRADIATION_MODE_PWM_INSIDE_COLD_DEFAULT;
						pwm_inside_warm_want = DEEP_IRRADIATION_MODE_PWM_INSIDE_WARM_DEFAULT;
						break;
					}
					default:
						break;
				}
				break;			
			case LED_6:
				switch(sys.mode)	
				{
					case NORMAL_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_6_LU_default +
										(outside_cold_pwm_6_RU_default - outside_cold_pwm_6_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_6_LD_default +
										(outside_cold_pwm_6_RD_default - outside_cold_pwm_6_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);

						color_temp_up   = 0.5 + outside_warm_pwm_6_LU_default -
										(outside_warm_pwm_6_LU_default - outside_warm_pwm_6_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_6_LD_default -
										(outside_warm_pwm_6_LD_default - outside_warm_pwm_6_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);
						pwm_inside_cold_want = pwm_outside_cold_want;
						pwm_inside_warm_want = pwm_outside_warm_want;
						break;
					}
					case DEEP_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_6_LU_default +
										(outside_cold_pwm_6_RU_default - outside_cold_pwm_6_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_6_LD_default +
										(outside_cold_pwm_6_RD_default - outside_cold_pwm_6_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 1.2 * (0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1));
						if(pwm_outside_cold_want > 1000)
						{
							pwm_outside_cold_want = 1000;
						}
					
						color_temp_up   = 0.5 + outside_warm_pwm_6_LU_default -
										(outside_warm_pwm_6_LU_default - outside_warm_pwm_6_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_6_LD_default -
										(outside_warm_pwm_6_LD_default - outside_warm_pwm_6_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 1.2 * (0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1));
						if(pwm_outside_warm_want > 1000)
						{
							pwm_outside_warm_want = 1000;
						}
						
						pwm_inside_cold_want = pwm_outside_cold_want;
						pwm_inside_warm_want = pwm_outside_warm_want;
						break;
					}
					case BACK_MODE:
					{
						pwm_outside_cold_want = 0;
						pwm_outside_warm_want = 0;
						pwm_inside_cold_want = 1000;
						pwm_inside_warm_want = 1000;
						break;
					}
					case SURFACE_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_6_LU_default +
										(outside_cold_pwm_6_RU_default - outside_cold_pwm_6_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_6_LD_default +
										(outside_cold_pwm_6_RD_default - outside_cold_pwm_6_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);

						color_temp_up   = 0.5 + outside_warm_pwm_6_LU_default -
										(outside_warm_pwm_6_LU_default - outside_warm_pwm_6_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_6_LD_default -
										(outside_warm_pwm_6_LD_default - outside_warm_pwm_6_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);
						pwm_inside_cold_want = 0;
						pwm_inside_warm_want = 0;
						break;
					}
					case DEEP_IRRADIATION_MODE:
					{
						pwm_outside_cold_want = DEEP_IRRADIATION_MODE_PWM_OUTSIDE_COLD_DEFAULT;
						pwm_outside_warm_want = DEEP_IRRADIATION_MODE_PWM_OUTSIDE_WARM_DEFAULT;
						pwm_inside_cold_want = DEEP_IRRADIATION_MODE_PWM_INSIDE_COLD_DEFAULT;
						pwm_inside_warm_want = DEEP_IRRADIATION_MODE_PWM_INSIDE_WARM_DEFAULT;
						break;
					}
					default:
						break;
				}
				break;
			case LED_4:
				switch(sys.mode)
				{
					case NORMAL_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_4_LU_default +
										(outside_cold_pwm_4_RU_default - outside_cold_pwm_4_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_4_LD_default +
										(outside_cold_pwm_4_RD_default - outside_cold_pwm_4_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);

						color_temp_up   = 0.5 + outside_warm_pwm_4_LU_default -
										(outside_warm_pwm_4_LU_default - outside_warm_pwm_4_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_4_LD_default -
										(outside_warm_pwm_4_LD_default - outside_warm_pwm_4_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);
						pwm_inside_cold_want = pwm_outside_cold_want;	
						pwm_inside_warm_want = pwm_outside_warm_want;
						break;
					}
					case DEEP_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_4_LU_default +
										(outside_cold_pwm_4_RU_default - outside_cold_pwm_4_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_4_LD_default +
										(outside_cold_pwm_4_RD_default - outside_cold_pwm_4_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 1.2 * (0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1));
						if(pwm_outside_cold_want > 1000)
						{
							pwm_outside_cold_want = 1000;
						}
					
						color_temp_up   = 0.5 + outside_warm_pwm_4_LU_default -
										(outside_warm_pwm_4_LU_default - outside_warm_pwm_4_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_4_LD_default -
										(outside_warm_pwm_4_LD_default - outside_warm_pwm_4_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 1.2 * (0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1));
						if(pwm_outside_warm_want > 1000)
						{
							pwm_outside_warm_want = 1000;
						}
						
						pwm_inside_cold_want = pwm_outside_cold_want;	
						pwm_inside_warm_want = pwm_outside_warm_want;
						break;
					}
					case BACK_MODE:
					{
						pwm_outside_cold_want = 0;
						pwm_outside_warm_want = 0;
						pwm_inside_cold_want = 1000;
						pwm_inside_warm_want = 1000;
						break;
					}
					case SURFACE_MODE:
					{
						color_temp_up   = 0.5 + outside_cold_pwm_4_LU_default +
										(outside_cold_pwm_4_RU_default - outside_cold_pwm_4_LU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_cold_pwm_4_LD_default +
										(outside_cold_pwm_4_RD_default - outside_cold_pwm_4_LD_default)/99.0*(sys.temp-1);
						pwm_outside_cold_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);

						color_temp_up   = 0.5 + outside_warm_pwm_4_LU_default -
										(outside_warm_pwm_4_LU_default - outside_warm_pwm_4_RU_default)/99.0*(sys.temp-1);
						color_temp_down = 0.5 + outside_warm_pwm_4_LD_default -
										(outside_warm_pwm_4_LD_default - outside_warm_pwm_4_RD_default)/99.0*(sys.temp-1);
						pwm_outside_warm_want = 0.5 + color_temp_up + (color_temp_down - color_temp_up)/99.0*(sys.lum-1);
						pwm_inside_cold_want = 0;
						pwm_inside_warm_want = 0;
						break;
					}
					case DEEP_IRRADIATION_MODE:
					{
						pwm_outside_cold_want = DEEP_IRRADIATION_MODE_PWM_OUTSIDE_COLD_DEFAULT;
						pwm_outside_warm_want = DEEP_IRRADIATION_MODE_PWM_OUTSIDE_WARM_DEFAULT;
						pwm_inside_cold_want = DEEP_IRRADIATION_MODE_PWM_INSIDE_COLD_DEFAULT;
						pwm_inside_warm_want = DEEP_IRRADIATION_MODE_PWM_INSIDE_WARM_DEFAULT;
						break;
					}
					default:
						break;
				}
				break;
			default:
				break;
		}
		if(sys.red_sw == true)
		{
			pwm_red_want = (uint16_t)sys.red * 10;
		}
		else
		{
			pwm_red_want = 0;
		}
		if(sys.green_sw == true)	
		{
			pwm_green_want = (uint16_t)sys.green * 10;
		}
		else
		{
			pwm_green_want = 0;
		}
	}
	else	//关机所有值都为0
	{
		pwm_outside_cold_want = 0;
		pwm_outside_warm_want = 0;
		pwm_inside_cold_want = 0;
		pwm_inside_warm_want = 0;
		pwm_red_want = 0;
		pwm_green_want = 0;
	}

	if(pwm_outside_cold_want_calculate != pwm_outside_cold_want)		
	{
		pwm_outside_cold_want_calculate = pwm_outside_cold_want;
		if(pwm_outside_cold_now > pwm_outside_cold_want)
		{
			pwm_outside_cold_change_temp =  (pwm_outside_cold_now - pwm_outside_cold_want) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_outside_cold_change_temp == 0)
			{
				pwm_outside_cold_change_temp = 1;
			}
		}
		else
		{
			pwm_outside_cold_change_temp =  (pwm_outside_cold_want - pwm_outside_cold_now) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_outside_cold_change_temp == 0)
			{
				pwm_outside_cold_change_temp = 1;
			}
		}
	}
	if(pwm_outside_warm_want_calculate != pwm_outside_warm_want)
	{
		pwm_outside_warm_want_calculate = pwm_outside_warm_want;
		if(pwm_outside_warm_now > pwm_outside_warm_want)
		{
			pwm_outside_warm_change_temp =  (pwm_outside_warm_now - pwm_outside_warm_want) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_outside_warm_change_temp == 0)
			{
				pwm_outside_warm_change_temp = 1;
			}
		}
		else
		{
			pwm_outside_warm_change_temp =  (pwm_outside_warm_want - pwm_outside_warm_now) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_outside_warm_change_temp == 0)
			{
				pwm_outside_warm_change_temp = 1;
			}
		}
	}
	if(pwm_inside_cold_want_calculate != pwm_inside_cold_want)
	{
		pwm_inside_cold_want_calculate = pwm_inside_cold_want;
		if(pwm_inside_cold_now > pwm_inside_cold_want)
		{
			pwm_inside_cold_change_temp =  (pwm_inside_cold_now - pwm_inside_cold_want) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_inside_cold_change_temp == 0)
			{
				pwm_inside_cold_change_temp = 1;
			}
		}
		else
		{
			pwm_inside_cold_change_temp =  (pwm_inside_cold_want - pwm_inside_cold_now) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_inside_cold_change_temp == 0)
			{
				pwm_inside_cold_change_temp = 1;
			}
		}
	}
	if(pwm_inside_warm_want_calculate != pwm_inside_warm_want)
	{
		pwm_inside_warm_want_calculate = pwm_inside_warm_want;
		if(pwm_inside_warm_now > pwm_inside_warm_want)
		{
			pwm_inside_warm_change_temp =  (pwm_inside_warm_now - pwm_inside_warm_want) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_inside_warm_change_temp == 0)
			{
				pwm_inside_warm_change_temp = 1;
			}
		}
		else
		{
			pwm_inside_warm_change_temp =  (pwm_inside_warm_want - pwm_inside_warm_now) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_inside_warm_change_temp == 0)
			{
				pwm_inside_warm_change_temp = 1;
			}
		}
	}
	if(pwm_red_want_calculate != pwm_red_want)
	{
		pwm_red_want_calculate = pwm_red_want;
		if(pwm_red_now > pwm_red_want)
		{
			pwm_red_change_temp =  (pwm_red_now - pwm_red_want) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_red_change_temp == 0)
			{
				pwm_red_change_temp = 1;
			}
		}
		else
		{
			pwm_red_change_temp =  (pwm_red_want - pwm_red_now) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_red_change_temp == 0)
			{
				pwm_red_change_temp = 1;
			}
		}
	}
	if(pwm_green_want_calculate != pwm_green_want)
	{
		pwm_green_want_calculate = pwm_green_want;
		if(pwm_green_now > pwm_green_want)
		{
			pwm_green_change_temp =  (pwm_green_now - pwm_green_want) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_green_change_temp == 0)
			{
				pwm_green_change_temp = 1;
			}
		}
		else
		{
			pwm_green_change_temp =  (pwm_green_want - pwm_green_now) / (sys.pwm_change_time/PWM_CTRL_TICK_DEFAULT);
			if(pwm_green_change_temp == 0)
			{
				pwm_green_change_temp = 1;
			}
		}
	}
	if(pwm_outside_cold_now != pwm_outside_cold_want)		
	{
		if(sys.pwm_change_mode == PWM_CHANGE_FAST)	//无渐变
		{
			pwm_outside_cold_now = pwm_outside_cold_want;
		}
		else	//灯光渐变
		{
			if(pwm_outside_cold_now > pwm_outside_cold_want)	
			{
				if((pwm_outside_cold_now-pwm_outside_cold_want) > pwm_outside_cold_change_temp)
				{
					pwm_outside_cold_now -= pwm_outside_cold_change_temp;
				}
				else
				{
					pwm_outside_cold_now = pwm_outside_cold_want;
				}
			}
			else	
			{
				if((pwm_outside_cold_want-pwm_outside_cold_now) > pwm_outside_cold_change_temp)
				{
					pwm_outside_cold_now += pwm_outside_cold_change_temp;
				}
				else
				{
					pwm_outside_cold_now = pwm_outside_cold_want;
				}
			}
		}
		switch(sys.ID)
		{
			case LED_700:
				huart1_send(OUTSIDE_COLD_PWM, pwm_outside_cold_now);
				break;
			case LED_500:
				huart1_send(OUTSIDE_COLD_PWM, pwm_outside_cold_now);
				break;
			case LED_6:
				if(sys.mode == DEEP_MODE)
				{
					huart1_send(FLOWER_OUTSIDE_CENTER_COLD_PWM, pwm_outside_cold_now);
					huart1_send(FLOWER_OUTSIDE_OUT_COLD_PWM, 0);
				}
				else
				{
					huart1_send(FLOWER_OUTSIDE_CENTER_COLD_PWM, pwm_outside_cold_now);
					huart1_send(FLOWER_OUTSIDE_OUT_COLD_PWM, pwm_outside_cold_now);
				}
				break;
			case LED_4:
				huart1_send(FLOWER_OUTSIDE_CENTER_COLD_PWM, pwm_outside_cold_now);
				huart1_send(FLOWER_OUTSIDE_OUT_COLD_PWM, pwm_outside_cold_now);
				break;
			default:
				break;
		}
	}
	if(pwm_outside_warm_now != pwm_outside_warm_want)
	{
		if(sys.pwm_change_mode == PWM_CHANGE_FAST)
		{
			pwm_outside_warm_now = pwm_outside_warm_want;
		}
		else
		{
			if(pwm_outside_warm_now > pwm_outside_warm_want)
			{
				if((pwm_outside_warm_now-pwm_outside_warm_want) > pwm_outside_warm_change_temp)	
				{
					pwm_outside_warm_now -= pwm_outside_warm_change_temp;
				}
				else
				{
					pwm_outside_warm_now = pwm_outside_warm_want;
				}
			}
			else
			{
				if((pwm_outside_warm_want-pwm_outside_warm_now) > pwm_outside_warm_change_temp)
				{
					pwm_outside_warm_now += pwm_outside_warm_change_temp;
				}
				else
				{
					pwm_outside_warm_now = pwm_outside_warm_want;
				}
			}
		}
		switch(sys.ID)
		{
			case LED_700:
				huart1_send(OUTSIDE_WARM_PWM, pwm_outside_warm_now);
				break;
			case LED_500:
				huart1_send(OUTSIDE_WARM_PWM, pwm_outside_warm_now);
				break;
			case LED_6:
				if(sys.mode == DEEP_MODE)
				{
					huart1_send(FLOWER_OUTSIDE_CENTER_WARM_PWM, pwm_outside_warm_now);
					huart1_send(FLOWER_OUTSIDE_OUT_WARM_PWM, 0);
				}
				else
				{
					huart1_send(FLOWER_OUTSIDE_CENTER_WARM_PWM, pwm_outside_warm_now);
					huart1_send(FLOWER_OUTSIDE_OUT_WARM_PWM, pwm_outside_warm_now);
				}
				break;
			case LED_4:
				huart1_send(FLOWER_OUTSIDE_CENTER_WARM_PWM, pwm_outside_warm_now);
				huart1_send(FLOWER_OUTSIDE_OUT_WARM_PWM, pwm_outside_warm_now);
				break;
			default:
				break;
		}
	}
	if(pwm_inside_cold_now != pwm_inside_cold_want)	
	{

		if(sys.pwm_change_mode == PWM_CHANGE_FAST)
		{
			pwm_inside_cold_now = pwm_inside_cold_want;
		}
		else
		{
			if(pwm_inside_cold_now > pwm_inside_cold_want)
			{
				if((pwm_inside_cold_now-pwm_inside_cold_want) > pwm_inside_cold_change_temp)	
				{
					pwm_inside_cold_now -= pwm_inside_cold_change_temp;
				}
				else
				{
					pwm_inside_cold_now = pwm_inside_cold_want;
				}
			}
			else
			{
				if((pwm_inside_cold_want-pwm_inside_cold_now) > pwm_inside_cold_change_temp)
				{
					pwm_inside_cold_now += pwm_inside_cold_change_temp;
				}
				else
				{
					pwm_inside_cold_now = pwm_inside_cold_want;
				}
			}
		}
		switch(sys.ID)
		{
			case LED_700:
				huart1_send(INSIDE_PWM, pwm_inside_cold_now);
				break;
			case LED_500:
				huart1_send(INSIDE_PWM, pwm_inside_cold_now);
				break;
			case LED_6:
				huart1_send(FLOWER_CENTER_COLD_PWM, pwm_inside_cold_now);
				break;
			case LED_4:
				huart1_send(FLOWER_CENTER_COLD_PWM, pwm_inside_cold_now);
				break;
			default:
				break;
		}					
	}
	
	if(pwm_inside_warm_now != pwm_inside_warm_want)	
	{
		if(sys.pwm_change_mode == PWM_CHANGE_FAST)
		{
			pwm_inside_warm_now = pwm_inside_warm_want;
		}
		else
		{
			if(pwm_inside_warm_now > pwm_inside_warm_want)
			{
				if((pwm_inside_warm_now-pwm_inside_warm_want) > pwm_inside_warm_change_temp)	
				{
					pwm_inside_warm_now -= pwm_inside_warm_change_temp;
				}
				else
				{
					pwm_inside_warm_now = pwm_inside_warm_want;
				}
			}
			else
			{
				if((pwm_inside_warm_want-pwm_inside_warm_now) > pwm_inside_warm_change_temp)
				{
					pwm_inside_warm_now += pwm_inside_warm_change_temp;
				}
				else
				{
					pwm_inside_warm_now = pwm_inside_warm_want;
				}
			}
		}
		switch(sys.ID)
		{
			case LED_6:
				huart1_send(FLOWER_CENTER_WARM_PWM, pwm_inside_warm_now);
				break;
			case LED_4:
				huart1_send(FLOWER_CENTER_WARM_PWM, pwm_inside_warm_now);
				break;
			default:
				break;
		}
	}

	if(pwm_red_now != pwm_red_want)	
	{
		if(sys.pwm_change_mode == PWM_CHANGE_FAST)
		{
			pwm_red_now = pwm_red_want;
		}
		else
		{
			if(pwm_red_now > pwm_red_want)
			{
				if((pwm_red_now-pwm_red_want) > pwm_red_change_temp)
				{
					pwm_red_now -= pwm_red_change_temp;
				}
				else
				{
					pwm_red_now = pwm_red_want;
				}
			}
			else
			{
				if((pwm_red_want-pwm_red_now) > pwm_red_change_temp)
				{
					pwm_red_now += pwm_red_change_temp;
				}
				else
				{
					pwm_red_now = pwm_red_want;
				}
			}
		}
		switch(sys.ID)
		{
			case LED_700:
				huart1_send(RED_PWM, pwm_red_now);
				break;
			case LED_500:
				huart1_send(RED_PWM, pwm_red_now);
				break;
			case LED_6:
				huart1_send(FLOWER_RED_PWM, pwm_red_now);
				break;
			case LED_4:
				huart1_send(FLOWER_RED_PWM, pwm_red_now);
				break;
			default:
				break;
		}
	}
	
	if(pwm_green_now != pwm_green_want)
	{
		if(sys.pwm_change_mode == PWM_CHANGE_FAST)
		{
			pwm_green_now = pwm_green_want;
		}
		else
		{
			if(pwm_green_now > pwm_green_want)
			{
				if((pwm_green_now-pwm_green_want) > pwm_green_change_temp)	
				{
					pwm_green_now -= pwm_green_change_temp;
				}
				else
				{
					pwm_green_now = pwm_green_want;
				}
			}
			else
			{
				if((pwm_green_want-pwm_green_now) > pwm_green_change_temp)	
				{
					pwm_green_now += pwm_green_change_temp;
				}
				else
				{
					pwm_green_now = pwm_green_want;
				}
			}
		}
		switch(sys.ID)
		{
			case LED_700:
				huart1_send(GREEN_PWM, pwm_green_now);
				break;
			case LED_500:
				huart1_send(GREEN_PWM, pwm_green_now);
				break;
			case LED_6:
				huart1_send(FLOWER_GREEN_PWM, pwm_green_now);
				break;
			case LED_4:
				huart1_send(FLOWER_GREEN_PWM, pwm_green_now);
				break;
			default:
				break;
		}
	}
}


