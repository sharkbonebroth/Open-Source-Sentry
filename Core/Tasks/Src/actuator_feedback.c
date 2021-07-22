/*
 * actuator_feedback_task.c
 *
 *  Created on: Jan 19, 2021
 *      Author: Hans Kurnia
 */

#include "board_lib.h"
#include "robot_config.h"
#include <actuator_feedback.h>



uint16_t history[10000] = {0};
uint16_t history_index = 0;
can_data_t canone_data;

extern osEventFlagsId_t gimbal_data_flag;
extern osEventFlagsId_t chassis_data_flag;
extern osEventFlagsId_t gun_data_flag;

/**
 * CAN ISR function, triggered upon RX_FIFO0_MSG_PENDING
 * converts the raw can data to the motor_data struct form as well
 */
void can_ISR(CAN_HandleTypeDef *hcan)
{
	HAL_CAN_DeactivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_FULL | CAN_IT_RX_FIFO0_OVERRUN);
	if (hcan->Instance == CAN1)
	{
		CAN_RxHeaderTypeDef rx_msg_header;
		uint8_t rx_buffer[CAN_BUFFER_SIZE];
		can_get_msg(&hcan1, &rx_msg_header, rx_buffer);
		convert_raw_can_data(rx_msg_header.StdId, rx_buffer);
		history[history_index++] = rx_msg_header.StdId;
		if (history_index >= 10000)
		{
			history_index = 0;
		}
		HAL_CAN_ActivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_FULL	| CAN_IT_RX_FIFO0_OVERRUN);
	}
	else if (hcan->Instance == CAN2)
	{
		CAN_RxHeaderTypeDef rx_msg_header;
		uint8_t rx_buffer[CAN_BUFFER_SIZE];
		can_get_msg(&hcan2, &rx_msg_header, rx_buffer);
		convert_raw_can_data(rx_msg_header.StdId, rx_buffer);
		history[history_index++] = rx_msg_header.StdId;
		if (history_index >= 10000)
		{
			history_index = 0;
		}

		HAL_CAN_ActivateNotification(&hcan2, CAN_IT_RX_FIFO0_MSG_PENDING | CAN_IT_RX_FIFO0_FULL	| CAN_IT_RX_FIFO0_OVERRUN);
		// something
	}
}

/*
 * MOTOR IDS				PHYSICAL ID		ARRAY NO.
 * 0x201 -> ID_CHASSIS	 	ID1				0
 * 0x205 -> ID_FEEDER_R		ID5				1
 * 0x206 -> ID_FEEDER_L		ID6				2
 * 0x209 -> ID_PITCH		ID5	(on can2)	3
 * 0x20A -> ID_YAW			ID6	(on can2)   4
 *
 * Converts raw CAN data over to the motor_data_t struct
 * 7 bytes of CAN data is sent from the motors:
 * High byte for motor angle data
 * Low byte for motor angle data
 * High byte for RPM
 * Low byte for RPM
 * High byte for Torque
 * Low byte for Torque
 * 1 byte for temperature
 *
 * This function combines the respective high and low bytes into 1 single 16bit integer, then stores them
 * in the struct for the motor.
 *
 * For GM6020 motors, it recenters the motor angle data and converts it to radians.
 */

void convert_raw_can_data(uint16_t motor_id, uint8_t *rx_buffer)
{
	switch(motor_id)
	{
		uint8_t feeder_id;
		case ID_CHASSIS:
		{
			canone_data.CHASSIS.id 					= motor_id;
			canone_data.CHASSIS.angle				= (rx_buffer[0] << 8) | rx_buffer[1];
			canone_data.CHASSIS.rpm  				= (rx_buffer[2] << 8) | rx_buffer[3];
			canone_data.CHASSIS.torque 				= (rx_buffer[4] << 8) | rx_buffer[5];
			canone_data.CHASSIS.temp 				= (rx_buffer[6]);
			osEventFlagsSet(chassis_data_flag, 0x10);
		}

		case ID_FEEDER_R:
		case ID_FEEDER_L:
		{
			feeder_id = motor_id - 0x205;
			canone_data.FEEDER[feeder_id].id 			= motor_id;
			canone_data.FEEDER[feeder_id].angle			= (rx_buffer[0] << 8) | rx_buffer[1];
			canone_data.FEEDER[feeder_id].rpm  			= (rx_buffer[2] << 8) | rx_buffer[3];
			canone_data.FEEDER[feeder_id].torque		= (rx_buffer[4] << 8) | rx_buffer[5];
			canone_data.FEEDER[feeder_id].temp 			= (rx_buffer[6]);
			osEventFlagsSet(gun_data_flag, 0x10);
			break;
		}

		case ID_PITCH:
		{
			canone_data.pitch.id 						= motor_id;
			canone_data.pitch.real_angle				= (rx_buffer[0] << 8) | rx_buffer[1];
			canone_data.pitch.rpm  						= (rx_buffer[2] << 8) | rx_buffer[3];
			canone_data.pitch.torque 					= (rx_buffer[4] << 8) | rx_buffer[5];
			canone_data.pitch.temp 						= (rx_buffer[6]);
			gimbal_offset(&canone_data.pitch);
			osEventFlagsSet(gimbal_data_flag, 0x10);
			break;
		}


		case ID_YAW:
		{
			canone_data.yaw.id 							= motor_id;
			canone_data.yaw.real_angle					= (rx_buffer[0] << 8) | rx_buffer[1];
			canone_data.yaw.rpm  						= (rx_buffer[2] << 8) | rx_buffer[3];
			canone_data.yaw.torque 						= (rx_buffer[4] << 8) | rx_buffer[5];
			canone_data.yaw.temp 						= (rx_buffer[6]);
			gimbal_offset(&canone_data.yaw);
			osEventFlagsSet(gimbal_data_flag, 0x01);
			break;
		}
		default:
		{
			break;
		}
	}
}

/**
 * Centers the raw motor angle to between -Pi to +Pi
 */
void gimbal_offset(gimbal_data_t *gimbal_data)
{
	gimbal_data->adj_ang = gimbal_data->real_angle - gimbal_data->center_ang;
	gimbal_data->adj_ang = (float)gimbal_data->adj_ang/(GM6020_MAX_DEFLECTION) * PI; // convert to radians
	if (gimbal_data->adj_ang < -PI)
	{
		gimbal_data->adj_ang += 2 * PI;
	}
	else if (gimbal_data->adj_ang > PI)
	{
		gimbal_data->adj_ang -= 2 * PI;
	}
}
