/*
 * input_control_task.c
 *
 *  Created on: Jan 19, 2021
 *      Author: Hans Kurnia
 */
#include "board_lib.h"
#include "robot_config.h"
#include "rc_input.h"

extern osThreadId_t gimbal_control_task_handle;
extern osThreadId_t movement_control_task_handle;
extern osEventFlagsId_t rc_data_flag;

#define JOYSTICK_OFFSET 1024
remote_cmd_t remote_cmd = {0};
uint8_t remote_control_mode = 0;

extern float pitch;
extern float yaw;


//Releases dbus semaphore when a dbus signal is detected by ISR
void dbus_remote_ISR(DMA_HandleTypeDef *hdma) {
	remote_cmd.right_x = (remote_raw_data[0] | remote_raw_data[1] << 8) & 0x07FF;
	remote_cmd.right_x -= JOYSTICK_OFFSET;
	remote_cmd.right_y = (remote_raw_data[1] >> 3 | remote_raw_data[2] << 5) & 0x07FF;
	remote_cmd.right_y -= JOYSTICK_OFFSET;
	remote_cmd.left_x = (remote_raw_data[2] >> 6 | remote_raw_data[3] << 2 | remote_raw_data[4] << 10) & 0x07FF;
	remote_cmd.left_x -= JOYSTICK_OFFSET;
	remote_cmd.left_y = (remote_raw_data[4] >> 1 | remote_raw_data[5] << 7) & 0x07FF;
	remote_cmd.left_y -= JOYSTICK_OFFSET;
	//Left switch position
	remote_cmd.left_switch = ((remote_raw_data[5] >> 4) & 0x000C) >> 2;
	remote_cmd.right_switch = (remote_raw_data[5] >> 4) & 0x0003;
	remote_cmd.mouse_x = ((int16_t)remote_raw_data[6] | ((int16_t)remote_raw_data[7] << 8));
	remote_cmd.mouse_y = ((int16_t)remote_raw_data[8] | ((int16_t)remote_raw_data[9] << 8));
	remote_cmd.mouse_z = ((int16_t)remote_raw_data[10] | ((int16_t)remote_raw_data[11] << 8));
	remote_cmd.mouse_left = (remote_raw_data[12]);
	remote_cmd.mouse_right = (remote_raw_data[13]);
	remote_cmd.keyboard_keys = (remote_raw_data[14]);
	remote_cmd.last_time = HAL_GetTick();
	if(remote_cmd.keyboard_keys & KEY_OFFSET_Q && remote_cmd.keyboard_keys & KEY_OFFSET_SHIFT
			&& remote_cmd.keyboard_keys & KEY_OFFSET_CTRL)
	{
		if (remote_control_mode == 1)
		{
			remote_control_mode = 0;
		}
		else
		{
			remote_control_mode = 1;
		}
	}
	osEventFlagsSet(rc_data_flag, 0x1000);
}

void dbus_reset()
{
	HAL_UART_DMAPause(&huart1);
	remote_cmd.right_x = 0;
	remote_cmd.right_y = 0;
	remote_cmd.left_x = 0;
	remote_cmd.left_y = 0;
	remote_cmd.left_switch = 0;
	remote_cmd.mouse_x = 0;
	remote_cmd.mouse_y = 0;
	remote_cmd.mouse_z = 0;
	remote_cmd.mouse_left = (remote_raw_data[12]);
	remote_cmd.mouse_right = (remote_raw_data[13]);
	pitch = 0;
	yaw = 0;
	HAL_UART_DMAResume(&huart1);
}
