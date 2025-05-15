#include "bsp.h"
#include "app_path.h"
#include "bsp_buzzer_led.h"


void BSP_Init(void)
{
	Bsp_Tim_Init();
	PID_Param_Init();//电机PID初始化 Motor PID initialization
	BSP_LED_Init();  // LED初始化
	APP_Path_Init(); // 路径控制初始化
	
	// 设置巡线速度
	set_line_speed(700);  // 设置中等速度
}


void BSP_Loop(void)
{
	// 更新蜂鸣器状态（非阻塞）
	BSP_Buzzer_Beep(0);
	
	// 更新路径控制逻辑
	APP_Path_Loop();
}
