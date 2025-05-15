/*
 * app_irtracking.c
 *
 *  Created on: Oct 25, 2023
 *      Author: YB-101
 */

#include "app_irtracking.h"

// 巡线传感器状态
uint8_t g_sensor_status = 0;
// 巡线速度控制（默认500）
int16_t g_line_speed =500;

/**
 * @brief  设置巡线基础速度
 * @param  speed: 速度值(0-1000)
 * @retval 无
 */
void set_line_speed(int16_t speed)
{
	if (speed > 0 && speed <= 1000)
	{
		g_line_speed = speed;
	}
}

/**
 * @brief  获取四路传感器状态的组合值
 * @param  无
 * @retval 传感器状态（4位二进制表示）
 */
uint8_t get_sensor_status(void)
{
	uint8_t status = 0;

	if (IN_X1 == 1)
		status |= 0x08; // 1000
	if (IN_X2 == 1)
		status |= 0x04; // 0100
	if (IN_X3 == 1)
		status |= 0x02; // 0010
	if (IN_X4 == 1)
		status |= 0x01; // 0001

	g_sensor_status = status;
	return status;
}

/**
 * @brief  改进的巡线控制函数
 * @param  无
 * @retval 无
 */
void car_irtrack(void)
{
	
	
	
	
	// 获取传感器状态
	uint8_t status = get_sensor_status();
	int16_t base_speed = g_line_speed;

	//测试代码
	/*switch (status)
	{
		case 0x08: // 1000 - 大幅偏右，急转向左
		Motion_Set_Speed(1000,0,0,0);
		break;
		case 0x01: // 0001 - 大幅偏左，急转向右
		Motion_Set_Speed(0,0,0,1000);
		break;
		case 0x04: // 0100 - 偏右，左转调整
		Motion_Set_Speed(0,1000,0,0);
		break;
		case 0x02: // 0010 - 偏左，右转调整
		Motion_Set_Speed(0,0,1000,0);
		break;

		
	}*/


	// 根据不同传感器状态调整电机速度
	switch (status)
	{
	case 0x06: // 0110 - 中间两个传感器都在线上，直行
		Motion_Set_Speed(base_speed, base_speed, base_speed, base_speed);
		break;

	case 0x02: // 0010 - 偏左，右转调整
		Motion_Set_Speed(0, 0, base_speed, base_speed);
		break;

	case 0x04: // 0100 - 偏右，左转调整
		Motion_Set_Speed(base_speed, base_speed, 0, 0);
		break;

	case 0x03: // 0011 - 更偏左，向右调整
		Motion_Set_Speed(0, 0, base_speed, base_speed);
		break;

	case 0x0C: // 1100 - 更偏右，向左调整
		Motion_Set_Speed(base_speed, base_speed, 0, 0);
		break;

	case 0x01: // 0001 - 大幅偏左，急转向右
		Motion_Set_Speed(-base_speed / 2, -base_speed / 2, base_speed, base_speed);
		break;

	case 0x08: // 1000 - 大幅偏右，急转向左
		Motion_Set_Speed(base_speed, base_speed, -base_speed / 2, -base_speed / 2);
		break;

	case 0x0F: // 1111 - 所有传感器都在线上，可能是交叉点
		Motion_Set_Speed(base_speed, base_speed, base_speed, base_speed);
		break;

	case 0x00: // 0000 - 所有传感器都不在线上，可能丢线
		// 保持上一次的动作，避免突然停止
		break;

	case 0x09: // 1001 - 两侧都检测到，可能是特殊情况
		Motion_Set_Speed(base_speed / 2, base_speed / 2, base_speed / 2, base_speed / 2);
		break;

	default: // 其它情况，适当减速直行
		Motion_Set_Speed(base_speed / 2, base_speed / 2, base_speed / 2, base_speed / 2);
		break;
	}
}

/**
 * @brief  专门用于弧线巡线的控制函数
 * @param  turn_direction: 0表示左转弧线，1表示右转弧线
 * @param  turn_radius: 弯曲半径系数(0-100)，越大弯道越大
 * @retval 无
 */
void car_arc_tracking(uint8_t turn_direction, uint8_t turn_radius)
{
	// 获取传感器状态
	uint8_t status = get_sensor_status();
	int16_t base_speed = g_line_speed;
	int16_t inner_speed, outer_speed;

	// 根据弧线方向和弯曲半径计算内外轮速度
	if (turn_radius > 100)
		turn_radius = 100;

	// 内轮速度为外轮速度的比例(0-100%)
	outer_speed = base_speed;
	inner_speed = base_speed * turn_radius / 100;

	if (turn_direction == 0) // 左转弧线
	{
		// 左侧为内轮
		if (status == 0x06 || status == 0x02 || status == 0x04) // 基本在线上
		{
			Motion_Set_Speed(inner_speed, inner_speed, outer_speed, outer_speed);
		}
		else if (status == 0x01 || status == 0x03) // 偏右，需要向左调整
		{
			Motion_Set_Speed(inner_speed / 2, inner_speed / 2, outer_speed, outer_speed);
		}
		else if (status == 0x08 || status == 0x0C) // 偏左，需要向右调整
		{
			Motion_Set_Speed(inner_speed, inner_speed, outer_speed / 2, outer_speed / 2);
		}
		else if (status == 0x00) // 丢线处理，继续转向
		{
			Motion_Set_Speed(inner_speed / 2, inner_speed / 2, outer_speed, outer_speed);
		}
		else // 其它情况
		{
			Motion_Set_Speed(inner_speed, inner_speed, outer_speed, outer_speed);
		}
	}
	else // 右转弧线
	{
		// 右侧为内轮
		if (status == 0x06 || status == 0x02 || status == 0x04) // 基本在线上
		{
			Motion_Set_Speed(outer_speed, outer_speed, inner_speed, inner_speed);
		}
		else if (status == 0x01 || status == 0x03) // 偏右，需要向左调整
		{
			Motion_Set_Speed(outer_speed, outer_speed, inner_speed / 2, inner_speed / 2);
		}
		else if (status == 0x08 || status == 0x0C) // 偏左，需要向右调整
		{
			Motion_Set_Speed(outer_speed / 2, outer_speed / 2, inner_speed, inner_speed);
		}
		else if (status == 0x00) // 丢线处理，继续转向
		{
			Motion_Set_Speed(outer_speed, outer_speed, inner_speed / 2, inner_speed / 2);
		}
		else // 其它情况
		{
			Motion_Set_Speed(outer_speed, outer_speed, inner_speed, inner_speed);
		}
	}
}
