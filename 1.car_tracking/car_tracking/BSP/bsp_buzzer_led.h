/*
 * bsp_buzzer_led.h
 *
 *  Created on: May 1, 2024
 *      Author: AutoCar
 */

#ifndef BSP_BUZZER_LED_H_
#define BSP_BUZZER_LED_H_

#include "main.h"

/* 蜂鸣器控制宏 */
#define BUZZER_ON()  HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET)
#define BUZZER_OFF() HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET)
#define BUZZER_TOGGLE() HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin)

/* 左侧RGB灯控制宏 */
#define LRGB_R_ON()  HAL_GPIO_WritePin(LRGB_R_GPIO_Port, LRGB_R_Pin, GPIO_PIN_SET)
#define LRGB_R_OFF() HAL_GPIO_WritePin(LRGB_R_GPIO_Port, LRGB_R_Pin, GPIO_PIN_RESET)
#define LRGB_G_ON()  HAL_GPIO_WritePin(LRGB_G_GPIO_Port, LRGB_G_Pin, GPIO_PIN_SET)
#define LRGB_G_OFF() HAL_GPIO_WritePin(LRGB_G_GPIO_Port, LRGB_G_Pin, GPIO_PIN_RESET)
#define LRGB_B_ON()  HAL_GPIO_WritePin(LRGB_B_GPIO_Port, LRGB_B_Pin, GPIO_PIN_SET)
#define LRGB_B_OFF() HAL_GPIO_WritePin(LRGB_B_GPIO_Port, LRGB_B_Pin, GPIO_PIN_RESET)

/* 右侧RGB灯控制宏 */
#define RRGB_R_ON()  HAL_GPIO_WritePin(RRGB_GPIO_Port, RRGB_R_Pin, GPIO_PIN_SET)
#define RRGB_R_OFF() HAL_GPIO_WritePin(RRGB_GPIO_Port, RRGB_R_Pin, GPIO_PIN_RESET)
#define RRGB_G_ON()  HAL_GPIO_WritePin(RRGB_GPIO_Port, RRGB_G_Pin, GPIO_PIN_SET)
#define RRGB_G_OFF() HAL_GPIO_WritePin(RRGB_GPIO_Port, RRGB_G_Pin, GPIO_PIN_RESET)
#define RRGB_B_ON()  HAL_GPIO_WritePin(RRGB_GPIO_Port, RRGB_B_Pin, GPIO_PIN_SET)
#define RRGB_B_OFF() HAL_GPIO_WritePin(RRGB_GPIO_Port, RRGB_B_Pin, GPIO_PIN_RESET)

/* 按键读取宏 */
#define KEY1_PRESSED() (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY1_Pin) == GPIO_PIN_RESET)
#define KEY2_PRESSED() (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY2_Pin) == GPIO_PIN_RESET)
#define KEY3_PRESSED() (HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY3_Pin) == GPIO_PIN_RESET)

/* 函数声明 */
void BSP_LED_Init(void);
void BSP_LED_All_OFF(void);
void BSP_LED_All_ON(void);
void BSP_LED_Set_Color(uint8_t left_r, uint8_t left_g, uint8_t left_b, 
                       uint8_t right_r, uint8_t right_g, uint8_t right_b);
void BSP_Buzzer_Beep(uint16_t time_ms);
void BSP_Notify_Point(void);  // 到达关键点的声光提示

#endif /* BSP_BUZZER_LED_H_ */ 