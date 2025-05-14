/*
 * bsp_buzzer_led.c
 *
 *  Created on: May 1, 2024
 *      Author: AutoCar
 */

#include "bsp_buzzer_led.h"

/**
 * @brief  初始化LED灯
 * @param  无
 * @retval 无
 */
void BSP_LED_Init(void)
{
    BSP_LED_All_OFF();
}

/**
 * @brief  关闭所有LED灯
 * @param  无
 * @retval 无
 */
void BSP_LED_All_OFF(void)
{
    LRGB_R_OFF();
    LRGB_G_OFF();
    LRGB_B_OFF();
    RRGB_R_OFF();
    RRGB_G_OFF();
    RRGB_B_OFF();
}

/**
 * @brief  打开所有LED灯
 * @param  无
 * @retval 无
 */
void BSP_LED_All_ON(void)
{
    LRGB_R_ON();
    LRGB_G_ON();
    LRGB_B_ON();
    RRGB_R_ON();
    RRGB_G_ON();
    RRGB_B_ON();
}

/**
 * @brief  设置两侧RGB灯颜色
 * @param  left_r: 左侧红色 (0或1)
 * @param  left_g: 左侧绿色 (0或1)
 * @param  left_b: 左侧蓝色 (0或1)
 * @param  right_r: 右侧红色 (0或1)
 * @param  right_g: 右侧绿色 (0或1)
 * @param  right_b: 右侧蓝色 (0或1)
 * @retval 无
 */
void BSP_LED_Set_Color(uint8_t left_r, uint8_t left_g, uint8_t left_b, 
                       uint8_t right_r, uint8_t right_g, uint8_t right_b)
{
    if (left_r) LRGB_R_ON(); else LRGB_R_OFF();
    if (left_g) LRGB_G_ON(); else LRGB_G_OFF();
    if (left_b) LRGB_B_ON(); else LRGB_B_OFF();
    
    if (right_r) RRGB_R_ON(); else RRGB_R_OFF();
    if (right_g) RRGB_G_ON(); else RRGB_G_OFF();
    if (right_b) RRGB_B_ON(); else RRGB_B_OFF();
}

/**
 * @brief  蜂鸣器发声一段时间
 * @param  time_ms: 发声时间 (毫秒)
 * @retval 无
 * @note   这是非阻塞版本
 */
void BSP_Buzzer_Beep(uint16_t time_ms)
{
    static uint32_t beep_start_time = 0;
    static uint8_t beep_active = 0;
    uint32_t current_time = HAL_GetTick();
    
    if (time_ms > 0 && !beep_active) {
        // 开始新的蜂鸣
        BUZZER_ON();
        beep_start_time = current_time;
        beep_active = 1;
    }
    
    // 检查是否需要关闭蜂鸣器
    if (beep_active && (current_time - beep_start_time >= time_ms)) {
        BUZZER_OFF();
        beep_active = 0;
    }
}

/**
 * @brief  到达关键点的声光提示
 * @param  无
 * @retval 无
 */
void BSP_Notify_Point(void)
{
    // 蜂鸣器响250毫秒
    BSP_Buzzer_Beep(250);
    
    // 闪烁RGB灯
    static uint32_t last_toggle_time = 0;
    static uint8_t toggle_count = 0;
    uint32_t current_time = HAL_GetTick();
    
    if (current_time - last_toggle_time >= 100) {
        // 每100ms切换一次
        if (toggle_count < 6) { // 闪烁3次 (开关共6次)
            // 切换所有灯
            HAL_GPIO_TogglePin(LRGB_R_GPIO_Port, LRGB_R_Pin);
            HAL_GPIO_TogglePin(LRGB_G_GPIO_Port, LRGB_G_Pin);
            HAL_GPIO_TogglePin(LRGB_B_GPIO_Port, LRGB_B_Pin);
            HAL_GPIO_TogglePin(RRGB_GPIO_Port, RRGB_R_Pin);
            HAL_GPIO_TogglePin(RRGB_GPIO_Port, RRGB_G_Pin);
            HAL_GPIO_TogglePin(RRGB_GPIO_Port, RRGB_B_Pin);
            
            toggle_count++;
            last_toggle_time = current_time;
        } else if (toggle_count == 6) {
            // 闪烁结束，关闭所有灯
            BSP_LED_All_OFF();
            toggle_count = 0;
        }
    }
} 