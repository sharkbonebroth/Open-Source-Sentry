/*
 * launcher_control_task.c
 *
 *  Created on: Jan 19, 2021
 *      Author: Hans Kurnia
 */

#include "board_lib.h"
#include "robot_config.h"
#include "actuator_feedback.h"
#include "rc_input.h"
#include "motor_control.h"
#include "gimbal_control_task.h"
#include "stdbool.h"

#define GEARBOX_RATIO 36

float pitch = 0;
float yaw = 0;
uint8_t xavier_rx_buffer[OBC_DATA_SIZE] = {0};
xavier_packet_t xavier_data;
float center_ang = 0;
float yaw_ang = 0;

bool sweep_right = true;
bool sweep_up = true;
bool sweeping_yaw = true;

extern remote_cmd_t remote_cmd;
extern can_data_t canone_data;
extern osEventFlagsId_t gimbal_data_flag;
extern osEventFlagsId_t rc_data_flag;
extern osThreadId_t movement_control_task_handle;

void xavier_ISR(DMA_HandleTypeDef *hdma)
{
	xavier_data.magic_number = ((xavier_rx_buffer[0] << 8) | xavier_rx_buffer[1]);
	xavier_data.x_pos = ((xavier_rx_buffer[2] << 8) | xavier_rx_buffer[3]);
	xavier_data.y_pos = (xavier_rx_buffer[4] << 8) | xavier_rx_buffer[5];
	xavier_data.end_check = (xavier_rx_buffer[6] << 8) | xavier_rx_buffer[7];
	if (xavier_data.magic_number != START_MAGIC_NUMBER || xavier_data.end_check != END_MAGIC_NUMBER)
	{
		xavier_data.y_pos = 0;
		xavier_data.x_pos = 0;
	}
	else
	{
		xavier_data.last_time = HAL_GetTick();
	}
}
/**
 *
 * FreeRTOS task for gimbal controls
 * Has HIGH2 priority
 *
 */
void gimbal_control_task(void *argument)
{
	HAL_UART_Receive_DMA(&XAVIER_UART, xavier_rx_buffer, OBC_DATA_SIZE);
    while(1)
    {
		gimbal_angle_control(&canone_data.pitch, &canone_data.yaw);
		vTaskDelay(1);
    }
   	osThreadTerminate(NULL);
}

/**
 * This function controls the gimbals based on motor's centered angle reading
 * @param 	pitch_motor		Pointer to pitch motor struct
 * 			yaw_motor		Pointer to yaw motor struct
 * works...decently enough. Need to retune PID for each robot.
 *
 * @note both pitch and yaw are currently on CAN2 with ID5 and 6.
 * Need to check if having ID4 (i.e. 0x208) + having the launcher motors (ID 1-3, 0x201 to 0x203)
 * still provides a fast enough response for open source robots
 */

void yaw_sweep(gimbal_data_t *pitch_motor, gimbal_data_t *yaw_motor)
{
	pitch = -0.1;
	if (sweep_right == true)
	{
		yaw += YAW_SWEEP_SPEED;
		if (yaw > yaw_motor->max_ang)
		{
			sweep_right = false;
		}
	}
	else
	{
		yaw -= YAW_SWEEP_SPEED;
		if (yaw < yaw_motor->min_ang)
		{
			sweep_right = true;
		}
	}
}




void gimbal_angle_control(gimbal_data_t *pitch_motor, gimbal_data_t *yaw_motor)
{
	if (remote_cmd.right_switch == random_movement)
	{
		yaw_sweep(pitch_motor, yaw_motor);
	}
	else if (remote_cmd.left_switch == aimbot_enable && xavier_data.last_time + XAVIER_TIMEOUT < HAL_GetTick())
	{
		//If Xavier has not timed out, move gimbal towards target
		//Normalize xavier data
		float normalized_y = xavier_data.y_pos + XAVIER_PITCH_NORMALIZER;
		float normalized_x = xavier_data.x_pos + XAVIER_YAW_NORMALIZER;
		pitch += normalized_y * XAVIER_SWEEP_SPEED;
		yaw += normalized_x + XAVIER_SWEEP_SPEED;
	}
	else if (remote_cmd.left_switch == teleopetate /*|| xavier_data.last_time + XAVIER_TIMEOUT < HAL_GetTick()*/)
	{
		if (HAL_GetTick() - remote_cmd.last_time > REMOTE_TIMEOUT )
		{
			dbus_reset();
		}
		pitch += (float)remote_cmd.right_y/660 * PITCH_SPEED * PITCH_INVERT;
		yaw += (float)remote_cmd.right_x/660 * YAW_SPEED * YAW_INVERT;
	}

	if (pitch > pitch_motor->max_ang)
	{
		pitch = pitch_motor->max_ang;
	}
	if (pitch < pitch_motor->min_ang)
	{
		pitch = pitch_motor->min_ang;
	}


	if (yaw > yaw_motor->max_ang)
	{
		yaw = yaw_motor->max_ang;
	}
	if (yaw < yaw_motor->min_ang)
	{
		yaw = yaw_motor->min_ang;
	}

	angle_pid(pitch, pitch_motor->adj_ang, pitch_motor);
	angle_pid(yaw, yaw_motor->adj_ang, yaw_motor);
	CANtwo_cmd(pitch_motor->pid.output, yaw_motor->pid.output, 0, 0, GIMBAL_ID);
}

