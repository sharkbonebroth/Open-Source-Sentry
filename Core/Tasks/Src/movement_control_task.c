/*
 * movement_control_task.c
 *
 *  Created on: Jul 28, 2021
 *      Author: Zheng Hao
 */


#include "board_lib.h"
#include "robot_config.h"
#include "motor_control.h"
#include "arm_math.h"
#include "rc_input.h"
#include "movement_control_task.h"
#include "stdlib.h"
#include "robot_config.h"
#include "gimbal_control_task.h"

extern remote_cmd_t remote_cmd;
extern can_data_t canone_data;
extern xavier_packet_t xavier_data;
extern uint8_t aimbot_mode;
extern osEventFlagsId_t chassis_data_flag;


float current_position = 0;
float track_length = 0;
float last_position_update_time = 0;
float goal_position = 0;
float last_speed_commanded = 0;
bool moving_towards_goal = false;
bool stopping = false;

void movement_control_task(void *argument)
{
	while(1)
	{
		if (canone_data.CHASSIS.torque > HOMING_TORQUE) //Uses a bump to the end of the track as an indicator of hitting a known point along the track, and updates current position
		{
			if (last_speed_commanded > 0)
			{
				current_position = track_length;
			}
			else
			{
				current_position = 0;
			}
		}
		update_current_position(false); //Computes current position along the track by adding the speed of the wheel
		if (remote_cmd.left_switch != kill)
		{
			if(remote_cmd.left_switch == teleopetate)
			{
				chassis_motion_control(&canone_data.CHASSIS);
			}
			else if (remote_cmd.right_switch == random_movement)
			{
				chassis_sweep(&canone_data.CHASSIS);
			}
			else
			{
				// Stops chassis movement if aimbot decides to standby or fire launcher
				last_speed_commanded = 0;
				speed_pid(0, canone_data.CHASSIS.rpm, &canone_data.CHASSIS.pid);
				CANone_cmd(canone_data.CHASSIS.pid.output, 0, 0, 0, CHASSIS_ID);
			}
		}
		else
		{
			CANone_cmd(0,0,0,0,CHASSIS_ID);
		}
		//delays task for other tasks to run
		vTaskDelay(CHASSIS_DELAY);
	}
	osThreadTerminate(NULL);
}


float update_current_position(bool homing) //Updates the current position of the robot along the track, and returns the change in position from the previous time the function was called
{
	float current_time;
	float time_difference;
	float change_in_position;
	float new_current_position;
	new_current_position = current_position;
	current_time = HAL_GetTick();
	time_difference = (current_time - last_position_update_time)/ 1000; //gives time difference in seconds
	last_position_update_time = current_time;
	change_in_position = rpm_to_speed(canone_data.CHASSIS.rpm) * time_difference; //gives change in position in meters
	new_current_position += change_in_position;
	if (homing != true) //Checks if the robot is not homing. If so, prevents the robot from integrating above the track length or below 0
	{
		if (new_current_position > track_length)
		{
			change_in_position = track_length - current_position;
			new_current_position = track_length;
		}
		if (new_current_position < 0)
		{
			change_in_position = 0 - current_position;
			new_current_position = 0;
		}
	}
	current_position = new_current_position;
	return change_in_position;
}

float rpm_to_speed(float rpm) //Returns speed of chassis in meters per second
{
	float speed;
	speed = rpm / 60 * (3.14159 * WHEEL_DIAMETER);
	return speed;
}

void chassis_motion_control(motor_data_t *motor)
{
	int16_t out_wheel = 0;
	out_wheel = (MAX_SPEED * remote_cmd.left_x)/(MAX_RC_VALUE/2);
	if (out_wheel > 0 && current_position >= (track_length - 0.15)) //Prevents the sentry from going too close to the limits
	{
		out_wheel = 0;
	}
	else if (out_wheel < 0 && current_position <= 0.15)
	{
		out_wheel = 0;
	}
	last_speed_commanded = out_wheel;
	speed_pid(out_wheel, motor->rpm, &motor->pid);
	CANone_cmd(motor->pid.output, 0, 0, 0, CHASSIS_ID);
}

void chassis_sweep(motor_data_t *motor)
{
	if (stopping == true) //Ensure that the chassis has stopped before a new sweeping cycle begins
	{
		if (motor->rpm == 0)
		{
			stopping = false;
		}
		else
		{
			last_speed_commanded = 0;
			speed_pid(0, motor->rpm, &motor->pid);
			CANone_cmd(motor->pid.output, 0, 0, 0, CHASSIS_ID);
		}
	}
	else if (moving_towards_goal == false)
	{
		generate_goal_position();
		moving_towards_goal = true;
	}
	else
	{
		float distance_from_current_to_goal;
		distance_from_current_to_goal = goal_position - current_position;
		if (fabs(distance_from_current_to_goal) < 0.05)
		{
			moving_towards_goal = false;
			stopping = true;
		}
		else if (distance_from_current_to_goal > 0)
		{
			last_speed_commanded = SWEEPING_SPEED;
			speed_pid(SWEEPING_SPEED, motor->rpm, &motor->pid);
			CANone_cmd(motor->pid.output, 0, 0, 0, CHASSIS_ID);
		}
		else
		{
			last_speed_commanded = -SWEEPING_SPEED;
			speed_pid(-SWEEPING_SPEED, motor->rpm, &motor->pid);
			CANone_cmd(motor->pid.output, 0, 0, 0, CHASSIS_ID);
		}
	}
}

void generate_goal_position() //generates goal positions with 0.15m of leeway from max/min goal positions to the limits
{
	goal_position = 0.15 + (rand() / RAND_MAX) * (track_length - 0.3);
}

void homing_sequence(motor_data_t *motor) //Homing sequence to measure the length of the track and start the sentry from a home point
{
	home_gimbal(&canone_data.pitch, &canone_data.yaw); //Home gimbal before homing
	current_position = 0;
	track_length = 0;
	// Move to one end of the track
	while (motor->torque < HOMING_TORQUE)
	{
		last_speed_commanded = -HOMING_SPEED;
		speed_pid(-HOMING_SPEED, motor->rpm, &motor->pid);
		CANone_cmd(motor->pid.output, 0, 0, 0, CHASSIS_ID);
	}
	// Start computing the length of the track while moving to the other end of the track
	while (motor->torque < HOMING_TORQUE)
	{
		last_speed_commanded = HOMING_SPEED;
		speed_pid(HOMING_SPEED, motor->rpm, &motor->pid);
		CANone_cmd(motor->pid.output, 0, 0, 0, CHASSIS_ID);
		track_length += update_current_position(true);
	}
}
