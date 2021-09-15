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
#include "robot_config.h"

extern remote_cmd_t remote_cmd;
extern can_data_t canone_data;
extern xavier_packet_t xavier_data;
extern uint8_t aimbot_mode;
extern osEventFlagsId_t chassis_data_flag;

/*static uint32_t start_time = 0;
static uint8_t unjamming = 0;*/

float current_position = START_POSITION;
uint16_t prev_chassis_motor_angle;

void movement_control_task(void *argument)
{
	//prev_chassis_motor_angle = canone_data.CHASSIS->angle;
	while(1)
	{
		if (remote_cmd.left_switch != kill)
		{
			if(remote_cmd.left_switch == teleopetate)
			{
				osEventFlagsWait(chassis_data_flag, 0x10, osFlagsWaitAll, 100);
				chassis_motion_control(canone_data.CHASSIS);
				osEventFlagsClear(chassis_data_flag, 0x10);
			}
			else if (remote_cmd.right_switch == random_movement)
			{
				// TODO RANDOM MOVEMENT
				osEventFlagsWait(chassis_data_flag, 0x10, osFlagsWaitAll, 100);
				//chassis_sweep(canone_data.CHASSIS);
				osEventFlagsClear(chassis_data_flag, 0x10);
			}
			else
			{
				// Stops chassis movement if aimbot decides to standby or fire launcher
				CANone_cmd(0,0,0,0,CHASSIS_ID);
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

/*
void update_current_position()
{
	if (canone_data.CHASSIS->rpm < 0)
	{

	}
}
*/

//Movement restricted to along x axis (hence, only read in remote_cmd.left_x)
void chassis_motion_control(motor_data_t *motor)
{
	int16_t out_wheel = 0;
	out_wheel = MAX_SPEED * (remote_cmd.left_x)/(MAX_RC_VALUE/2);
	speed_pid(out_wheel,motor->rpm, &motor->pid);
	CANone_cmd(motor->pid.output, 0, 0, 0, CHASSIS_ID);
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

void chassis_sweep(motor_data_t *motor)
{

}

