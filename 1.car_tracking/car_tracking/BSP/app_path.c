/*
 * app_path.c
 *
 *  Created on: May 1, 2024
 *      Author: AutoCar
 */

#include "app_path.h"

static CarMode_t current_mode = MODE_IDLE;
static PathPoint_t current_point = POINT_A;
static ArcState_t current_arc = ARC_NONE;
static uint8_t task4_lap_count = 0;
static uint32_t task_start_time = 0;
static uint8_t task_completed = 0;

/**
 * @brief  初始化路径控制模块
 * @param  无
 * @retval 无
 */
void APP_Path_Init(void)
{
    // 初始化模式为空闲
    current_mode = MODE_IDLE;
    current_point = POINT_A;
    current_arc = ARC_NONE;
    task4_lap_count = 0;
    task_completed = 0;
    
    // 初始化LED
    BSP_LED_Init();
    
    // 设置初始LED状态 - 待机模式蓝色呼吸灯
    BSP_LED_Set_Color(0, 0, 1, 0, 0, 1);
}

/**
 * @brief  路径控制主循环
 * @param  无
 * @retval 无
 */
void APP_Path_Loop(void)
{
    // 检查按键输入
    APP_Check_Button();
    
    // 根据当前模式执行相应任务
    switch (current_mode) {
        case MODE_IDLE:
            // 空闲模式，等待按键选择
            break;
            
        case MODE_TASK1:
            APP_Task1_Process();
            break;
            
        case MODE_TASK2:
            APP_Task2_Process();
            break;
            
        case MODE_TASK3:
            APP_Task3_Process();
            break;
            
        case MODE_TASK4:
            APP_Task4_Process();
            break;
            
        default:
            // 未知模式，恢复到空闲
            current_mode = MODE_IDLE;
            break;
    }
    
    // 检查路线中的关键点
    if (current_mode != MODE_IDLE && !task_completed) {
        APP_Check_Points();
    }
}

/**
 * @brief  检查按键状态
 * @param  无
 * @retval 无
 */
void APP_Check_Button(void)
{
    static uint32_t last_key_time = 0;
    uint32_t current_time = HAL_GetTick();
    
    // 按键防抖动，限制按键检测频率
    if (current_time - last_key_time < 300) {
        return;
    }
    
    // 如果当前有任务正在进行且未完成，忽略按键
    if (current_mode != MODE_IDLE && !task_completed) {
        return;
    }
    
    // 检查按键1 - 任务1
    if (KEY1_PRESSED()) {
        APP_Set_Mode(MODE_TASK1);
        last_key_time = current_time;
    }
    // 检查按键2 - 任务2
    else if (KEY2_PRESSED()) {
        APP_Set_Mode(MODE_TASK2);
        last_key_time = current_time;
    }
    // 检查按键3 - 任务3或4（长按为任务4）
    else if (KEY3_PRESSED()) {
        // 简单长按检测
        HAL_Delay(500);
        if (KEY3_PRESSED()) {
            // 长按，选择任务4
            APP_Set_Mode(MODE_TASK4);
        } else {
            // 短按，选择任务3
            APP_Set_Mode(MODE_TASK3);
        }
        last_key_time = current_time;
    }
}

/**
 * @brief  设置当前模式
 * @param  mode: 要设置的模式
 * @retval 无
 */
void APP_Set_Mode(CarMode_t mode)
{
    // 停止小车
    Motion_Stop(1);
    
    // 更新模式
    current_mode = mode;
    current_point = POINT_A;  // 所有任务都从A点开始
    current_arc = ARC_NONE;
    task4_lap_count = 0;
    task_completed = 0;
    
    // 记录开始时间
    task_start_time = HAL_GetTick();
    
    // 根据模式设置LED颜色
    switch (mode) {
        case MODE_IDLE:
            BSP_LED_Set_Color(0, 0, 1, 0, 0, 1);  // 蓝色
            break;
        case MODE_TASK1:
            BSP_LED_Set_Color(1, 0, 0, 1, 0, 0);  // 红色
            break;
        case MODE_TASK2:
            BSP_LED_Set_Color(0, 1, 0, 0, 1, 0);  // 绿色
            break;
        case MODE_TASK3:
            BSP_LED_Set_Color(1, 1, 0, 1, 1, 0);  // 黄色
            break;
        case MODE_TASK4:
            BSP_LED_Set_Color(1, 0, 1, 1, 0, 1);  // 紫色
            break;
    }
    
    // 如果不是空闲模式，给出启动提示
    if (mode != MODE_IDLE) {
        BSP_Notify_Point();  // 起点提示
    }
}

/**
 * @brief  任务1处理函数 - A点到B点
 * @param  无
 * @retval 无
 */
void APP_Task1_Process(void)
{
    if (task_completed) {
        return;
    }
    
    // 根据当前状态执行不同操作
    switch (current_point) {
        case POINT_A:
            // 已经在A点，直接沿直线前进，巡线到B点
            car_irtrack();
            break;
            
        case POINT_B:
            // 到达B点，任务完成
            Motion_Stop(1);
            BSP_Notify_Point();  // B点提示
            
            // 检查时间限制
            if (HAL_GetTick() - task_start_time <= 15000) {
                // 在规定时间内完成
                BSP_LED_Set_Color(0, 1, 0, 0, 1, 0);  // 绿色表示成功
            } else {
                // 超时
                BSP_LED_Set_Color(1, 0, 0, 1, 0, 0);  // 红色表示超时
            }
            
            task_completed = 1;
            break;
            
        default:
            // 意外状态，回到A点重新开始
            current_point = POINT_A;
            break;
    }
}

/**
 * @brief  任务2处理函数 - A->B->C->D->A
 * @param  无
 * @retval 无
 */
void APP_Task2_Process(void)
{
    if (task_completed) {
        return;
    }
    
    // 根据当前状态执行不同操作
    switch (current_point) {
        case POINT_A:
            // 从A点前进到B点
            car_irtrack();
            break;
            
        case POINT_B:
            // 在B点，需要沿弧线BC行驶
            current_arc = ARC_BC;
            APP_Arc_Tracking(current_arc);
            break;
            
        case POINT_C:
            // 从C点前进到D点
            car_irtrack();
            break;
            
        case POINT_D:
            // 在D点，需要沿弧线DA行驶
            current_arc = ARC_DA;
            APP_Arc_Tracking(current_arc);
            break;
            
        default:
            // 意外状态，回到A点重新开始
            current_point = POINT_A;
            current_arc = ARC_NONE;
            break;
    }
    
    // 检查是否完成任务
    if (current_point == POINT_A && current_arc == ARC_NONE) {
        // 回到A点且不在弧线上，表示完成一圈
        Motion_Stop(1);
        BSP_Notify_Point();  // A点提示
        
        // 检查时间限制
        if (HAL_GetTick() - task_start_time <= 30000) {
            // 在规定时间内完成
            BSP_LED_Set_Color(0, 1, 0, 0, 1, 0);  // 绿色表示成功
        } else {
            // 超时
            BSP_LED_Set_Color(1, 0, 0, 1, 0, 0);  // 红色表示超时
        }
        
        task_completed = 1;
    }
}

/**
 * @brief  任务3处理函数 - A->C->B->D->A
 * @param  无
 * @retval 无
 */
void APP_Task3_Process(void)
{
    if (task_completed) {
        return;
    }
    
    // 根据当前状态执行不同操作
    switch (current_point) {
        case POINT_A:
            if (current_arc == ARC_NONE) {
                // 从A点前进到C点
                car_irtrack();
            } else if (current_arc == ARC_DA) {
                // 沿弧线DA行驶
                APP_Arc_Tracking(current_arc);
            }
            break;
            
        case POINT_C:
            // 在C点，需要沿弧线CB行驶
            current_arc = ARC_CB;
            APP_Arc_Tracking(current_arc);
            break;
            
        case POINT_B:
            // 从B点前进到D点
            car_irtrack();
            break;
            
        case POINT_D:
            // 在D点，需要沿弧线DA行驶
            current_arc = ARC_DA;
            APP_Arc_Tracking(current_arc);
            break;
            
        default:
            // 意外状态，回到A点重新开始
            current_point = POINT_A;
            current_arc = ARC_NONE;
            break;
    }
    
    // 检查是否完成任务
    if (current_point == POINT_A && current_arc == ARC_NONE) {
        // 回到A点且不在弧线上，表示完成一圈
        Motion_Stop(1);
        BSP_Notify_Point();  // A点提示
        
        // 检查时间限制
        if (HAL_GetTick() - task_start_time <= 40000) {
            // 在规定时间内完成
            BSP_LED_Set_Color(0, 1, 0, 0, 1, 0);  // 绿色表示成功
        } else {
            // 超时
            BSP_LED_Set_Color(1, 0, 0, 1, 0, 0);  // 红色表示超时
        }
        
        task_completed = 1;
    }
}

/**
 * @brief  任务4处理函数 - 按任务3路径行驶4圈
 * @param  无
 * @retval 无
 */
void APP_Task4_Process(void)
{
    if (task_completed) {
        return;
    }
    
    // 大部分处理逻辑与任务3相同
    switch (current_point) {
        case POINT_A:
            if (current_arc == ARC_NONE) {
                // 从A点前进到C点
                car_irtrack();
            } else if (current_arc == ARC_DA) {
                // 沿弧线DA行驶到A点
                APP_Arc_Tracking(current_arc);
                
                // 如果已到达A点且完成弧线
                if (current_point == POINT_A && current_arc == ARC_NONE) {
                    // 完成一圈，发出提示
                    BSP_Notify_Point();
                    
                    // 增加圈数
                    task4_lap_count++;
                    
                    // 如果未完成4圈，继续前进
                    if (task4_lap_count < 4) {
                        // 短暂停顿后继续
                        HAL_Delay(500);
                    } else {
                        // 完成4圈，停车
                        Motion_Stop(1);
                        // 检查完成时间
                        uint32_t total_time = HAL_GetTick() - task_start_time;
                        
                        // 设置完成指示灯 - 任务4没有明确的时间限制，显示总时间
                        // 这里假设小于140秒为良好表现
                        if (total_time <= 140000) {
                            BSP_LED_Set_Color(0, 1, 0, 0, 1, 0);  // 绿色表示良好
                        } else {
                            BSP_LED_Set_Color(1, 1, 0, 1, 1, 0);  // 黄色表示一般
                        }
                        
                        task_completed = 1;
                    }
                }
            }
            break;
            
        case POINT_C:
            // 在C点，需要沿弧线CB行驶
            current_arc = ARC_CB;
            APP_Arc_Tracking(current_arc);
            break;
            
        case POINT_B:
            // 从B点前进到D点
            car_irtrack();
            break;
            
        case POINT_D:
            // 在D点，需要沿弧线DA行驶
            current_arc = ARC_DA;
            APP_Arc_Tracking(current_arc);
            break;
            
        default:
            // 意外状态，回到A点重新开始当前圈
            current_point = POINT_A;
            current_arc = ARC_NONE;
            break;
    }
}

/**
 * @brief  检查当前是否到达关键点
 * @param  无
 * @retval 无
 */
void APP_Check_Points(void)
{
    // 注：这是一个简化的实现，实际需要根据场地情况和传感器状态来判断
    // 在真实情况下，可能需要特定的标记或多个传感器的组合状态来判断
    
    // 检测特定传感器组合状态来判断路口
    // 例如，所有传感器都在线上可能表示到达路口
    if (IN_X1 == 1 && IN_X2 == 1 && IN_X3 == 1 && IN_X4 == 1) {
        // 根据当前位置和行驶方向判断可能到达的点
        
        // 任务1：判断B点
        if (current_mode == MODE_TASK1 && current_point == POINT_A) {
            current_point = POINT_B;
            BSP_Notify_Point();  // 到达B点提示
        }
        
        // 任务2：判断B点和D点
        else if (current_mode == MODE_TASK2) {
            if (current_point == POINT_A) {
                current_point = POINT_B;
                BSP_Notify_Point();  // 到达B点提示
            } else if (current_point == POINT_C) {
                current_point = POINT_D;
                BSP_Notify_Point();  // 到达D点提示
            }
        }
        
        // 任务3和4：判断C点和D点
        else if (current_mode == MODE_TASK3 || current_mode == MODE_TASK4) {
            if (current_point == POINT_A && current_arc == ARC_NONE) {
                current_point = POINT_C;
                BSP_Notify_Point();  // 到达C点提示
            } else if (current_point == POINT_B) {
                current_point = POINT_D;
                BSP_Notify_Point();  // 到达D点提示
            }
        }
    }
    
    // 检测弧线完成的情况
    if (current_arc != ARC_NONE) {
        // 通过特定的传感器状态判断弧线完成
        // 例如，如果经过一段弧线后，中间传感器又回到线上
        if (IN_X2 == 1 && IN_X3 == 1) {
            switch (current_arc) {
                case ARC_BC:
                    current_point = POINT_C;
                    current_arc = ARC_NONE;
                    BSP_Notify_Point();  // 到达C点提示
                    break;
                    
                case ARC_DA:
                    current_point = POINT_A;
                    current_arc = ARC_NONE;
                    // A点提示在完成任务时处理
                    break;
                    
                case ARC_CB:
                    current_point = POINT_B;
                    current_arc = ARC_NONE;
                    BSP_Notify_Point();  // 到达B点提示
                    break;
                    
                case ARC_AD:
                    current_point = POINT_D;
                    current_arc = ARC_NONE;
                    BSP_Notify_Point();  // 到达D点提示
                    break;
                    
                default:
                    break;
            }
        }
    }
}

/**
 * @brief  弧线巡线策略
 * @param  arc: 当前弧线
 * @retval 无
 */
void APP_Arc_Tracking(ArcState_t arc)
{
    // 根据不同的弧线调整巡线策略
    switch (arc) {
        case ARC_BC:
            // B→C弧线，左转弧线
            car_arc_tracking(0, 60);  // 左转，弯曲半径60%
            break;
            
        case ARC_DA:
            // D→A弧线，右转弧线
            car_arc_tracking(1, 60);  // 右转，弯曲半径60%
            break;
            
        case ARC_CB:
            // C→B弧线，右转弧线
            car_arc_tracking(1, 60);  // 右转，弯曲半径60%
            break;
            
        case ARC_AD:
            // A→D弧线，左转弧线
            car_arc_tracking(0, 60);  // 左转，弯曲半径60%
            break;
            
        default:
            // 默认使用标准巡线
            car_irtrack();
            break;
    }
} 