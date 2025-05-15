/*
 * app_irtracking.h
 *
 *  Created on: Oct 25, 2023
 *      Author: YB-101
 */

#ifndef APP_IRTRACKING_H_
#define APP_IRTRACKING_H_

#include <stdint.h>  /* 添加标准整数类型定义 */
#include "bsp_irtracking.h"
#include "app_motor.h"

/* 函数声明 */
void car_irtrack(void);
void car_arc_tracking(uint8_t turn_direction, uint8_t turn_radius);
uint8_t get_sensor_status(void);
void set_line_speed(int16_t speed);

/* 全局变量声明 */
extern uint8_t g_sensor_status;
extern int16_t g_line_speed;

#endif /* APP_IRTRACKING_H_ */
