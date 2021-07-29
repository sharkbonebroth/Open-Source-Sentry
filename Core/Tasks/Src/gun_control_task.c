/*
 * gun_control_task.c
 *
 *  Created on: Jul 15, 2021
 *      Author: yanling
 */

#include "board_lib.h"
#include "robot_config.h"
#include "motor_control.h"
#include "rc_input.h"
#include "arm_math.h"
#include "gun_control_task.h"
extern can_data_t canone_data;
extern remote_cmd_t remote_cmd;
extern osEventFlagsId_t gun_data_flag;

static uint32_t start_time[2] = {0};
static uint8_t unjamming[2] = {0};
uint16_t rotation_freq =  1000000/400 ;   //recommended 30-500Hz for snail 2305, current freq: 400Hz
//note: Prescaler = 84, hence, each bit takes 1 microsecond for the counter
//TODO: Declare as MACRO instead.
uint16_t min_pwm = 400;
uint16_t max_pwm = 2200;


//TODO: Check rotation frequency, change to autonomous (currently dependent on RC)
void gun_control_task(void *argument)
{
	//Starting PWM for 4 snail motors (grouped as 1&2 and 3&4),
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_1);	// PD12
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_2); 	// PD13 (reversed)
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_3);	// PD14
	HAL_TIM_PWM_Start(&htim4,TIM_CHANNEL_4); 	// PD15 (reversed)
	htim4.Instance->ARR = rotation_freq;

	//throttle range calibration
/*
		pwm_output(-1,max_pwm);
		vTaskDelay(CHASSIS_DELAY);
		pwm_output(-1, min_pwm);
		vTaskDelay(CHASSIS_DELAY);
		pwm_output(-1, 0); // -1 just means all set to 0
*/

	while(1)
	{
		//refresh dbus data
		if (HAL_GetTick() - remote_cmd.last_time > REMOTE_TIMEOUT )
		{
			dbus_reset();
		}
		//if firing and kill switch is not activated
		if(remote_cmd.right_switch == fire && remote_cmd.left_switch != kill)
		{
			osEventFlagsWait(gun_data_flag, 0x10, osFlagsWaitAll, 100);
			launcher_control(canone_data.FEEDER);
			osEventFlagsClear(gun_data_flag, 0x10);
		}
		//otherwise kill the launcher
		else
		{
			pwm_output(-1,0);
			CANone_cmd(0,0,0,0, LAUNCHER_ID);
		}
		//delays task for other tasks to run, CHECK THE VALUE
		vTaskDelay(CHASSIS_DELAY);
	}
	osThreadTerminate(NULL);
}

void launcher_control(motor_data_t *feeders)
{
	int16_t feeder_output[2];

	for (int i = 0; i < 2; i++)
	{
		//Remember to change one of the motor direction according to data sheet since PWM cannot change direction.
		pwm_output(i,cycle_to_pulse(50)); // 0-100 (max speed), 50: 50% (?) of maximum speed = 1300 microseconds pulsewidth
		if (fabs(feeders[i].torque) > FEEDER_JAM_TORQUE)
		{
			unjamming[i] = 1;
			start_time[i] = HAL_GetTick();
		}

		//unjamming needed, check what feeder output to give
		if (unjamming[i] == 1)
		{
			// feeder is unjamming itself successfully
			if (start_time[i] + FEEDER_UNJAM_TIME < HAL_GetTick())
			{
				unjamming[i] = 0;
				feeder_output[i] = FEEDER_SPEED;
			}
			// feeder is unable to unjam, hence, send unjam speed
			else
			{
				feeder_output[i] = FEEDER_UNJAM_SPD;
			}
		}
		else
		{
			feeder_output[i] = FEEDER_SPEED;
		}
		speed_pid(feeder_output[i] * 36,feeders[i].rpm, &feeders[i].pid);
	}
	CANone_cmd(feeders[0].pid.output, feeders[1].pid.output,0,0,LAUNCHER_ID);  //feeder M2006 id 5-6, identifier = 0x1ff
}



void pwm_output(uint8_t timergroup, uint16_t output)
{
	if (timergroup == 0)
	{
		htim4.Instance->CCR1 = output;
		htim4.Instance->CCR2 = output;
	}
	else if (timergroup == 1)
	{
		htim4.Instance->CCR3 = output;
		htim4.Instance->CCR4 = output;
	}
	else
	{
		htim4.Instance->CCR1 = output;
		htim4.Instance->CCR2 = output;
		htim4.Instance->CCR3 = output;
		htim4.Instance->CCR4 = output;
	}

}

uint16_t cycle_to_pulse(uint8_t cycle)
{
	return (int)(min_pwm + cycle*(max_pwm-min_pwm)/100) ;
}

