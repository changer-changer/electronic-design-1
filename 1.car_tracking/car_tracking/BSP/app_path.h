/*
 * app_path.h
 *
 *  Created on: May 1, 2024
 *      Author: AutoCar
 */

#ifndef BSP_APP_PATH_H_
#define BSP_APP_PATH_H_

#include "main.h"
#include "bsp_buzzer_led.h"
#include "app_motor.h"
#include "app_irtracking.h"

/* 小车行驶模式 */
typedef enum {
    MODE_IDLE = 0,       // 空闲模式，等待按键选择
    MODE_TASK1 = 1,      // 任务1：基础行驶，A点到B点
    MODE_TASK2 = 2,      // 任务2：环形路径1，A->B->C->D->A
    MODE_TASK3 = 3,      // 任务3：环形路径2，A->C->B->D->A
    MODE_TASK4 = 4       // 任务4：循环行驶，按任务3路径行驶4圈
} CarMode_t;

/* 路径点 */
typedef enum {
    POINT_NONE = 0,      // 非关键点
    POINT_A = 1,         // A点
    POINT_B = 2,         // B点
    POINT_C = 3,         // C点
    POINT_D = 4          // D点
} PathPoint_t;

/* 弧线状态 */
typedef enum {
    ARC_NONE = 0,        // 非弧线
    ARC_BC = 1,          // B→C弧线
    ARC_DA = 2,          // D→A弧线
    ARC_CB = 3,          // C→B弧线
    ARC_AD = 4           // A→D弧线
} ArcState_t;

/* 函数声明 */
void APP_Path_Init(void);
void APP_Path_Loop(void);
void APP_Check_Button(void);
void APP_Set_Mode(CarMode_t mode);
void APP_Task1_Process(void);
void APP_Task2_Process(void);
void APP_Task3_Process(void);
void APP_Task4_Process(void);
void APP_Check_Points(void);
void APP_Arc_Tracking(ArcState_t arc);

#endif /* BSP_APP_PATH_H_ */ 