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
    
    // 使用HAL_GPIO_ReadPin直接读取按键状态而不是宏
    GPIO_PinState key1_state = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY1_Pin);
    GPIO_PinState key2_state = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY2_Pin);
    GPIO_PinState key3_state = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY3_Pin);
    
    // 检查按键1 - 任务1 (按键为低电平表示按下)
    if (key1_state == GPIO_PIN_RESET) {
        APP_Set_Mode(MODE_TASK1);
        last_key_time = current_time;
    }
    // 检查按键2 - 任务2
    else if (key2_state == GPIO_PIN_RESET) {
        APP_Set_Mode(MODE_TASK2);
        last_key_time = current_time;
    }
    // 检查按键3 - 任务3或4（长按为任务4）
    else if (key3_state == GPIO_PIN_RESET) {
        // 简单长按检测
        HAL_Delay(500);
        key3_state = HAL_GPIO_ReadPin(KEY_GPIO_Port, KEY3_Pin);
        if (key3_state == GPIO_PIN_RESET) {
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
            // 第一个任务需要延时1秒再开始
            HAL_Delay(1000);
            BSP_Notify_Point();  // 延时后发出启动提示
            break;
        case MODE_TASK2:
            BSP_LED_Set_Color(0, 1, 0, 0, 1, 0);  // 绿色
            // 第二个任务需要延时0.5秒再开始
            HAL_Delay(500);
            BSP_Notify_Point();  // 延时后发出启动提示
            // 立即开始直行
            Motion_Set_Speed(700, 700, 700, 700);
            break;
        case MODE_TASK3:
            BSP_LED_Set_Color(1, 1, 0, 1, 1, 0);  // 黄色
            break;
        case MODE_TASK4:
            BSP_LED_Set_Color(1, 0, 1, 1, 0, 1);  // 紫色
            break;
    }
    
    // 对于除了任务1和任务2以外的任务，在启动时给出提示
    if (mode != MODE_IDLE && mode != MODE_TASK1 && mode != MODE_TASK2) {
        BSP_Notify_Point();  // 起点提示
    }
}

/**
 * @brief  任务1处理函数 - 传感器变化检测
 * @param  无
 * @retval 无
 */
void APP_Task1_Process(void)
{
    static uint8_t prev_sensor_status = 0;
    static uint8_t init_done = 0;
    static uint32_t start_time = 0;
    static uint8_t detection_active = 0;
    static uint32_t detection_time = 0;
    
    if (task_completed) {
        return;
    }
    
    uint32_t current_time = HAL_GetTick();
    
    // 第一次进入，记录初始传感器状态和启动时间
    if (!init_done) {
        prev_sensor_status = get_sensor_status();
        start_time = current_time;
        init_done = 1;
        
        // 开始直行
        Motion_Set_Speed(700, 700, 700, 700);
        return;
    }
    
    // 已经检测到变化且处于延时停车阶段
    if (detection_active) {
        // 继续直行
        Motion_Set_Speed(700, 700, 700, 700);
        
        // 检查是否已经延时0.5秒
        if (current_time - detection_time >= 400) {
            // 延时0.5秒后停车
            Motion_Stop(1);
            
            // 发出声光提示
            BSP_Notify_Point();
            
            // 使用橙色灯光表示完成
            BSP_LED_Set_Color(1, 1, 0, 1, 1, 0);
            
            task_completed = 1;
        }
        return;
    }
    
    // 获取当前传感器状态
    uint8_t current_status = get_sensor_status();
    
    // 在开始行驶后的第一秒内，忽略传感器变化
    if (current_time - start_time <= 1000) {
        // 更新传感器状态但不执行停车检测
        prev_sensor_status = current_status;
        
        // 继续直行
        Motion_Set_Speed(700, 700, 700, 700);
        return;
    }
    
    // 检测是否有任何传感器从1变为0
    uint8_t changed_to_zero = (prev_sensor_status & ~current_status);
    
    if (changed_to_zero) {
        // 有传感器从1变为0，记录检测时间并设置检测状态
        detection_active = 1;
        detection_time = current_time;
    }
    
    // 更新上一次的传感器状态
    prev_sensor_status = current_status;
    
    // 继续直行
    Motion_Set_Speed(700, 700, 700, 700);
}

/**
 * @brief  任务2处理函数 - 使用原始巡线逻辑并检测关键点
 * @param  无
 * @retval 无
 */
void APP_Task2_Process(void)
{
    static uint32_t start_time = 0;
    static uint8_t has_started = 0;
    static uint32_t last_state_time = 0;
    static uint8_t in_black_area = 0; // 0表示在白色区域，1表示在黑色区域
    static uint8_t point_a_count = 0; // 记录经过A点的次数
    static uint8_t prev_all_black = 0; // 上一次是否全为黑色
    static uint32_t prev_state_change_time = 0; // 上次状态变化时间
    static uint32_t white_area_start_time = 0; // 开始进入白色区域的时间
    static uint8_t white_area_confirmed = 0; // 是否确认已进入白色区域
    
    uint32_t current_time = HAL_GetTick();
    
    if (task_completed) {
        return;
    }
    
    // 首次进入，记录开始时间
    if (!has_started) {
        start_time = current_time;
        has_started = 1;
        last_state_time = current_time;
        
        // 假设开始时在A点（黑色区域）
        in_black_area = 1;
        point_a_count = 0;
        white_area_confirmed = 0;
    }
    
    // 获取当前传感器状态
    uint8_t all_black = (IN_X1==1 && IN_X2==1 && IN_X3==1 && IN_X4==1);
    uint8_t all_white = (IN_X1==0 && IN_X2==0 && IN_X3==0 && IN_X4==0);
    uint8_t mostly_white = (!IN_X1 && !IN_X2 && !IN_X3 && !IN_X4) || // 全白
                           (IN_X1 == 0 && IN_X2 == 0 && IN_X3 == 0) || // 左三个传感器为白
                           (IN_X2 == 0 && IN_X3 == 0 && IN_X4 == 0); // 右三个传感器为白
    
    // 白色区域的持续时间检测
    if (in_black_area && mostly_white) {
        // 从黑色区域刚进入白色区域，记录开始时间
        if (white_area_start_time == 0) {
            white_area_start_time = current_time;
        }
        
        // 检查是否已经在白色区域持续超过0.5秒
        if (!white_area_confirmed && (current_time - white_area_start_time > 200)) {
            white_area_confirmed = 1;
            in_black_area = 0;
            point_a_count++;
            BSP_Notify_Point(); // A点或C点声光提示
            prev_state_change_time = current_time;
            
            // 如果是第二次经过A点，停车
            if (point_a_count >= 2) {
                Motion_Stop(1);
                BSP_LED_Set_Color(0, 1, 0, 0, 1, 0); // 绿色表示成功
                task_completed = 1;
                return;
            }
        }
    } else if (white_area_confirmed && all_black) {
        // 确认从白色区域进入黑色区域
        white_area_confirmed = 0;
        white_area_start_time = 0;
        in_black_area = 1;
        
        // 只有在上次状态变化时间超过500ms后才触发B点或D点
        if (current_time - prev_state_change_time > 500) {
            BSP_Notify_Point(); // B点或D点声光提示
            prev_state_change_time = current_time;
        }
    } else if (!mostly_white) {
        // 如果不是主要白色区域，重置白色计时
        white_area_start_time = 0;
    }
    
    // 使用原始巡线逻辑 - 直接从app_irtracking.c移植
    if(IN_X1==1 && IN_X2==1 && IN_X3==1 && IN_X4==1)
    {
        Motion_Set_Speed(1000,1000,1000,1000);
    }
    
    if((IN_X1 == 0 && IN_X3 == 0) && IN_X2 == 1 && IN_X4 == 1) //直走 go straight
    {
        Motion_Set_Speed(300,300,300,300);
    }

    if(IN_X1 == 0 && IN_X3 == 1 && IN_X4 == 1 && IN_X2 == 1) //小幅度调整 small adjustment
    {
        Motion_Set_Speed(0,0,500,500);
    }
    else if(IN_X1 == 1 && IN_X3 == 0 && IN_X4 == 1 && IN_X2 == 1)
    {
        Motion_Set_Speed(500,500,0,0);
    }

    if(IN_X2 == 0 && IN_X3 == 1) //大幅度左右转 Turn left and right sharply
    {
        Motion_Set_Speed(-500,-500,500,500);
    }
    else if(IN_X4 == 0 && IN_X1 == 1)
    {
        Motion_Set_Speed(500,500,-500,-500);
    }
    
    // 设置任务最大运行时间为30秒
    if (current_time - start_time > 9416) {
        // 超过30秒，任务结束
        Motion_Stop(1);
        BSP_Notify_Point();  // 结束提示
        
        // 由于超时，显示红色提示灯
        BSP_LED_Set_Color(1, 1, 1, 1, 1, 1);  // 红色表示超时
        
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
    static uint32_t last_point_time = 0;
    uint32_t current_time = HAL_GetTick();
    
    // 防止短时间内重复检测关键点（至少间隔1秒）
    if (current_time - last_point_time < 1000) {
        return;
    }
    
    // 获取当前传感器状态
    uint8_t status = get_sensor_status();
    
    // 检测特定传感器组合状态来判断路口
    // 所有传感器都在线上可能表示到达路口
    if (status == 0x0F) {  // 0x0F = 1111，表示所有传感器都检测到黑线
        // 根据当前位置和行驶方向判断可能到达的点
        
        // 任务1：不做特殊处理，由APP_Task1_Process负责监控传感器变化
        
        // 任务2：判断B点和D点
        if (current_mode == MODE_TASK2) {
            if (current_point == POINT_A) {
                current_point = POINT_B;
                last_point_time = current_time;
                BSP_Notify_Point();  // 到达B点提示
            } else if (current_point == POINT_C) {
                current_point = POINT_D;
                last_point_time = current_time;
                BSP_Notify_Point();  // 到达D点提示
            }
        }
        
        // 任务3和4：判断C点和D点
        else if (current_mode == MODE_TASK3 || current_mode == MODE_TASK4) {
            if (current_point == POINT_A && current_arc == ARC_NONE) {
                current_point = POINT_C;
                last_point_time = current_time;
                BSP_Notify_Point();  // 到达C点提示
            } else if (current_point == POINT_B) {
                current_point = POINT_D;
                last_point_time = current_time;
                BSP_Notify_Point();  // 到达D点提示
            }
        }
    }
    
    // 检测弧线完成的情况
    if (current_arc != ARC_NONE) {
        // 判断弧线完成的条件，使用过渡区域传感器状态
        // 由于弧线结束时的传感器状态可能多种多样，我们检测特定的组合
        if ((status == 0x06 || status == 0x07 || status == 0x0E || status == 0x0F) && 
            (current_time - last_point_time > 2000)) {  // 要求从开始弧线行驶后至少经过2秒
            
            switch (current_arc) {
                case ARC_BC:
                    current_point = POINT_C;
                    current_arc = ARC_NONE;
                    last_point_time = current_time;
                    BSP_Notify_Point();  // 到达C点提示
                    break;
                    
                case ARC_DA:
                    current_point = POINT_A;
                    current_arc = ARC_NONE;
                    last_point_time = current_time;
                    // A点提示在完成任务时处理
                    break;
                    
                case ARC_CB:
                    current_point = POINT_B;
                    current_arc = ARC_NONE;
                    last_point_time = current_time;
                    BSP_Notify_Point();  // 到达B点提示
                    break;
                    
                case ARC_AD:
                    current_point = POINT_D;
                    current_arc = ARC_NONE;
                    last_point_time = current_time;
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