/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f0xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define MODE_Pin GPIO_PIN_1
#define MODE_GPIO_Port GPIOA
#define LEVEL_Pin GPIO_PIN_2
#define LEVEL_GPIO_Port GPIOA
#define TFT_DC_Pin GPIO_PIN_6
#define TFT_DC_GPIO_Port GPIOA
#define LANGUAGE_Pin GPIO_PIN_0
#define LANGUAGE_GPIO_Port GPIOB
#define LUM_SUB_Pin GPIO_PIN_2
#define LUM_SUB_GPIO_Port GPIOB
#define TFT_RST_Pin GPIO_PIN_8
#define TFT_RST_GPIO_Port GPIOA
#define LUM_ADD_Pin GPIO_PIN_3
#define LUM_ADD_GPIO_Port GPIOB
#define TEMP_SUB_Pin GPIO_PIN_4
#define TEMP_SUB_GPIO_Port GPIOB
#define TEMP_ADD_Pin GPIO_PIN_5
#define TEMP_ADD_GPIO_Port GPIOB
#define R11_Pin GPIO_PIN_7
#define R11_GPIO_Port GPIOB
#define R9_Pin GPIO_PIN_8
#define R9_GPIO_Port GPIOB
#define ON_OFF_Pin GPIO_PIN_9
#define ON_OFF_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
