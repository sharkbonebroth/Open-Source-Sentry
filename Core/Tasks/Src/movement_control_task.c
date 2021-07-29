/*
 * movement_control_task.c
 *
 *  Created on: Jul 28, 2021
 *      Author: Hans Kurnia
 */


#include "board_lib.h"
#include "robot_config.h"
#include "motor_control.h"
#include "arm_math.h"
#include "rc_input.h"
#include "movement_control_task.h"
#include "stdlib.h"

extern remote_cmd_t remote_cmd;
extern can_data_t canone_data;
extern xavier_packet_t xavier_data;
extern uint8_t aimbot_mode;
extern osEventFlagsId_t chassis_data_flag;

/*static uint32_t start_time = 0;
static uint8_t unjamming = 0;*/


void movement_control_task(void *argument)
{
	while(1)
	{
		if(remote_cmd.left_switch == teleopetate)
		{
			osEventFlagsWait(chassis_data_flag, 0x10, osFlagsWaitAll, 100);
			chassis_motion_control(canone_data.CHASSIS);
			osEventFlagsClear(chassis_data_flag, 0x10);
		}
		else if (remote_cmd.right_switch == random_movement) //All on and aimbot on standby
		{
			// TODO RANDOM MOVEMENT
			osEventFlagsWait(chassis_data_flag, 0x10, osFlagsWaitAll, 100);
			osEventFlagsClear(chassis_data_flag, 0x10);
		}
		//delays task for other tasks to run
		vTaskDelay(CHASSIS_DELAY);
	}
	osThreadTerminate(NULL);
}

//Movement restricted to along x axis (hence, only read in remote_cmd.left_x)
void chassis_motion_control(motor_data_t *motor)
{
	// TODO
	/*
	//Holds wheel speed output, fl = front left, etc...
	int16_t out_wheel = 0;
	int8_t direction[2] = {-1,1};

	//Priority of switches, kill switch -> aimbot mode -> manual mode
	// Checks if kill switch activated before checking if its aimbot/manual mode. If manual mode, make sure that right switch is correct position
	// The particular ordering of conditions is to reflect the priority of the switches
	//e.g. This is not correct: right_switch != all_on -> update out_wheel and CANonecmd because this neglects aimbot mode
	// which should take priority over manual mode
	if (remote_cmd.right_switch == all_off)
	{
		CANone_cmd(0,0,0,0,CHASSIS_ID);
	}
	//TODO: tune the RNG is needed
	else if (remote_cmd.left_switch == aimbot_enable)
	{
		out_wheel = MAX_SPEED * (rand()%MAX_RC_VALUE)/(MAX_RC_VALUE) * direction[rand()%2];
		speed_pid(out_wheel,motor->rpm, &motor->pid);
		CANone_cmd(motor->pid.output, 0, 0, 0, CHASSIS_ID);
	}
	else if (remote_cmd.right_switch == all_on)
	{
		out_wheel = MAX_SPEED * (remote_cmd.left_x)/(MAX_RC_VALUE/2);
		speed_pid(out_wheel,motor->rpm, &motor->pid);
		CANone_cmd(motor->pid.output, 0, 0, 0, CHASSIS_ID);
	}
	else
	{
		CANone_cmd(0,0,0,0,CHASSIS_ID);
	*/

}


/*
//to OS sentry team: you can literally delete everything and just leave a multiplier for one of the joystick to
//control the chassis
//so once that's done maybe you can start on a system to move the sentry until the RNG says to not move
void chassis_motion_control(motor_data_t *motor)
{
	//Holds wheel speed output, fl = front left, etc...
	int16_t out_wheel[4] = {0,};
	int16_t highest_speed = 0;
	double rel_angle = 0;
	double control_yaw;
	float rel_x = 0;
	float rel_y = 0;

	rel_angle = canone_data.yaw.adj_ang;
	//rotation matrix of the initial vectorrrr wow MA1513 is relevant?!
	rel_x = (-remote_cmd.left_x * cos(-rel_angle)) + (remote_cmd.left_y * -sin(-rel_angle));
	rel_y = (-remote_cmd.left_x * sin(-rel_angle)) + (remote_cmd.left_y *  cos(-rel_angle));
	out_wheel[0] = (-rel_y - (rel_x * HORIZONTAL_MULT));
	out_wheel[1] = ( rel_y - (rel_x * HORIZONTAL_MULT));
	out_wheel[2] = ( rel_y + (rel_x * HORIZONTAL_MULT));
	out_wheel[3] = (-rel_y + (rel_x * HORIZONTAL_MULT));

	 normalize values as a percentage and multiple by our preset max speed
	 * this will be the setpoint for speed values
	 *
	if(canone_data.yaw.adj_ang >= (canone_data.yaw.max_ang) - 0.05)
	{
		if(aimbot_mode == 1)
		{
			if (xavier_data.yaw < 0)
			{
			control_yaw = xavier_data.yaw;
			}
		}
		else
		{
			if (remote_cmd.right_x < 0)
			{
			control_yaw = remote_cmd.right_x;
			}
		}
	}
	else if (canone_data.yaw.adj_ang <= canone_data.yaw.min_ang + 0.05)
	{
		if(aimbot_mode == 1)
			{
				if (xavier_data.yaw > 0)
				{
				control_yaw = xavier_data.yaw;
				}
			}
			else
			{
				if (remote_cmd.right_x > 0)
				{
				control_yaw = remote_cmd.right_x;
				}
			}
	}
	else
	{
		control_yaw = 0;
	}

	//limit max rpm
	for (uint8_t i = 0; i < 4; i ++)
	{
		out_wheel[i] = (out_wheel[i] * MAX_SPEED / (MAX_RC_VALUE/2)) + (control_yaw * TURNING_SPEED/MAX_RC_VALUE) ;
		if (fabs(out_wheel[i]) > highest_speed)
		{
			highest_speed = fabs(out_wheel[i]);
		}
	}

	if (fabs(highest_speed) > MAX_SPEED)
	{
		for (uint8_t i = 0; i < 4; i ++)
		{
			out_wheel[i] = out_wheel[i] * MAX_SPEED / highest_speed;
		}
	}

	for (uint8_t i = 0; i < 4; i ++)
	{
		speed_pid(out_wheel[i], motor[i].rpm , &motor[i].pid);
	}
	CANone_cmd(motor[0].pid.output, motor[1].pid.output, motor[2].pid.output, motor[3].pid.output, CHASSIS_ID);
}
*/
