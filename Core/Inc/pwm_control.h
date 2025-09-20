#ifndef __PWM_CONTROL_H__
#define __PWM_CONTROL_H__
#include "gpio.h"
#define outside_cold_pwm_500_LU_default 30
#define outside_cold_pwm_500_RU_default 380
#define outside_cold_pwm_500_LD_default 900
#define outside_cold_pwm_500_RD_default 930

#define outside_warm_pwm_500_LU_default 560
#define outside_warm_pwm_500_RU_default 225
#define outside_warm_pwm_500_LD_default 1000
#define outside_warm_pwm_500_RD_default 750




#define outside_cold_pwm_700_LU_default 40
#define outside_cold_pwm_700_RU_default 280
#define outside_cold_pwm_700_LD_default 80
#define outside_cold_pwm_700_RD_default 770

#define outside_warm_pwm_700_LU_default 297
#define outside_warm_pwm_700_RU_default 150
#define outside_warm_pwm_700_LD_default 850
#define outside_warm_pwm_700_RD_default 440



#define outside_cold_pwm_4_LU_default 0
#define outside_cold_pwm_4_RU_default 390
#define outside_cold_pwm_4_LD_default 0
#define outside_cold_pwm_4_RD_default 890

#define outside_warm_pwm_4_LU_default 255
#define outside_warm_pwm_4_RU_default 125
#define outside_warm_pwm_4_LD_default 500
#define outside_warm_pwm_4_RD_default 400


#define outside_cold_pwm_6_LU_default 0
#define outside_cold_pwm_6_RU_default 302
#define outside_cold_pwm_6_LD_default 0
#define outside_cold_pwm_6_RD_default 800

#define outside_warm_pwm_6_LU_default 200
#define outside_warm_pwm_6_RU_default 90
#define outside_warm_pwm_6_LD_default 630
#define outside_warm_pwm_6_RD_default 250

#define DEEP_IRRADIATION_MODE_PWM_OUTSIDE_COLD_DEFAULT 10
#define DEEP_IRRADIATION_MODE_PWM_OUTSIDE_WARM_DEFAULT 10
#define DEEP_IRRADIATION_MODE_PWM_INSIDE_COLD_DEFAULT 900
#define DEEP_IRRADIATION_MODE_PWM_INSIDE_WARM_DEFAULT 900


#define pwm_change_time_default 500
#define PWM_CTRL_TICK_DEFAULT 30

typedef enum
{
	BACK_MODE,
	DEEP_MODE,
	SURFACE_MODE,
	DEEP_IRRADIATION_MODE,
	NORMAL_MODE,
	MODE_NULL = 0xFF
}LedModeDef;

typedef enum
{
	PWM_CHANGE_FAST,
	PWM_CHANGE_SLOW,
	PWM_CHANGE_NULL = 0xFF
}PwmChangeDef;

typedef enum
{
	DATA_NULL,
	HAND_SHAKE,
	OUTSIDE_COLD_PWM,
	OUTSIDE_WARM_PWM,
	INSIDE_PWM,
	RED_PWM,
	GREEN_PWM,
	CAMERA_POWER,
	FLOWER_CENTER_WARM_PWM,
	FLOWER_CENTER_COLD_PWM,
	FLOWER_OUTSIDE_CENTER_WARM_PWM,
	FLOWER_OUTSIDE_CENTER_COLD_PWM,
	FLOWER_OUTSIDE_OUT_WARM_PWM,
	FLOWER_OUTSIDE_OUT_COLD_PWM,
	FLOWER_RED_PWM,
	FLOWER_GREEN_PWM,
	MY_DATA_TICK,
	VER_S
}MyDataDef;
void pwm_send_to_driverboard(void);
#endif
