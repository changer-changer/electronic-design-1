/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
#include "stm32f1xx_hal.h"

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
#define HAL_3A_Pin GPIO_PIN_0
#define HAL_3A_GPIO_Port GPIOA
#define HAL_3B_Pin GPIO_PIN_1
#define HAL_3B_GPIO_Port GPIOA
#define X1_Pin GPIO_PIN_13
#define X1_GPIO_Port GPIOF
#define X2_Pin GPIO_PIN_14
#define X2_GPIO_Port GPIOF
#define X3_Pin GPIO_PIN_15
#define X3_GPIO_Port GPIOF
#define X4_Pin GPIO_PIN_0
#define X4_GPIO_Port GPIOG

/* 按键定义 */
#define KEY1_Pin GPIO_PIN_3
#define KEY2_Pin GPIO_PIN_4
#define KEY3_Pin GPIO_PIN_5
#define KEY_GPIO_Port GPIOG

/* 蜂鸣器定义 */
#define BUZZER_Pin GPIO_PIN_12
#define BUZZER_GPIO_Port GPIOG

/* RGB灯定义 */
#define RRGB_R_Pin GPIO_PIN_2
#define RRGB_G_Pin GPIO_PIN_3
#define RRGB_B_Pin GPIO_PIN_4
#define LRGB_R_Pin GPIO_PIN_1
#define LRGB_G_Pin GPIO_PIN_7
#define LRGB_B_Pin GPIO_PIN_2
#define RRGB_GPIO_Port GPIOE
#define LRGB_R_GPIO_Port GPIOG
#define LRGB_G_GPIO_Port GPIOE
#define LRGB_B_GPIO_Port GPIOG


#define Motor3_A_Pin GPIO_PIN_9
#define Motor3_A_GPIO_Port GPIOE
#define Motor3_B_Pin GPIO_PIN_11
#define Motor3_B_GPIO_Port GPIOE



#define Motor4_A_Pin GPIO_PIN_13
#define Motor4_A_GPIO_Port GPIOE
#define Motor4_B_Pin GPIO_PIN_14
#define Motor4_B_GPIO_Port GPIOE



#define HAL_1A_Pin GPIO_PIN_12
#define HAL_1A_GPIO_Port GPIOD
#define HAL_1B_Pin GPIO_PIN_13
#define HAL_1B_GPIO_Port GPIOD



#define Motor1_A_Pin GPIO_PIN_6
#define Motor1_A_GPIO_Port GPIOC
#define Motor1_B_Pin GPIO_PIN_7
#define Motor1_B_GPIO_Port GPIOC


#define Motor2_A_Pin GPIO_PIN_8
#define Motor2_A_GPIO_Port GPIOC
#define Motor2_B_Pin GPIO_PIN_9
#define Motor2_B_GPIO_Port GPIOC



#define HAL_2A_Pin GPIO_PIN_15
#define HAL_2A_GPIO_Port GPIOA
#define HAL_2B_Pin GPIO_PIN_3
#define HAL_2B_GPIO_Port GPIOB
#define HAL_4A_Pin GPIO_PIN_4
#define HAL_4A_GPIO_Port GPIOB
#define HAL_4B_Pin GPIO_PIN_5
#define HAL_4B_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
