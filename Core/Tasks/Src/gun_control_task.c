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
extern xavier_packet_t xavier_data;

static uint32_t jam_time[2] = {0};
static uint32_t start_time[2] = {0};
static uint8_t unjamming[2] = {0};
static uint8_t firing[2] = {0};
uint16_t rotation_freq =  1000000/400 ;   //recommended 30-500Hz for snail 2305, current freq: 400Hz
//note: Prescaler = 84, hence, each bit takes 1 microsecond for the counter
//TODO: Declare as MACRO instead.
uint16_t min_pwm = 400;
uint16_t max_pwm = 2200;
float last_fed_time = 0;


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
			firing[0] = 0;
			firing[1] = 0;
		}
		//if firing and kill switch is not activated
		if(remote_cmd.right_switch == fire && remote_cmd.left_switch != kill)
		{
			if (remote_cmd.left_switch == aimbot_enable)
			{
				//If aimbot is enabled, only fire if the target is close to the center. Will shoot if the target is within a box of XAVIER_SHOOT_BOX_SIZE in the center of the camera
				// Normalize xavier data
				float normalized_y = xavier_data.y_pos + XAVIER_PITCH_NORMALIZER;
				float normalized_x = xavier_data.x_pos + XAVIER_YAW_NORMALIZER;
				if ((fabs(normalized_y) < (XAVIER_SHOOT_BOX_SIZE/2)) && (fabs(normalized_x) < (XAVIER_SHOOT_BOX_SIZE/2)))
				{
					osEventFlagsWait(gun_data_flag, 0x10, osFlagsWaitAll, 100);
					pewpew(canone_data.FEEDER);
					osEventFlagsClear(gun_data_flag, 0x10);
				}
			}
			else
			{
				// If teleoperating, fire based purely on right switch
				osEventFlagsWait(gun_data_flag, 0x10, osFlagsWaitAll, 100);
				pewpew(canone_data.FEEDER);
				osEventFlagsClear(gun_data_flag, 0x10);
			}
		}
		//otherwise kill the launcher
		else
		{
			// If not one sec has passed since the last feeding of launcher, continue spinning flywheel to prevent jamming unless the killswitch is flipped
			if (last_fed_time + 1000 > HAL_GetTick() && last_fed_time != 0)
			{
				// If kill switch is triggered, kill flywheels as well
				if (remote_cmd.left_switch == kill)
				{
					pwm_output(-1, 400);
				}
				else
				{
					pwm_output(-1, 1300);
				}
			}
			else
			{
				pwm_output(-1, 400);
			}
			speed_pid(0, canone_data.FEEDER[0].rpm, &canone_data.FEEDER[0].pid);
			speed_pid(0, canone_data.FEEDER[1].rpm, &canone_data.FEEDER[1].pid);
			CANone_cmd(canone_data.FEEDER[0].pid.output,canone_data.FEEDER[1].pid.output,0,0, LAUNCHER_ID);
			firing[0] = 0;
			firing[1] = 0;
		}
		//delays task for other tasks to run, CHECK THE VALUE
		vTaskDelay(CHASSIS_DELAY);
	}
	osThreadTerminate(NULL);
}

void pewpew(motor_data_t *feeders)
{
	int16_t feeder_output[2];

	for (int i = 0; i < 2; i++)
	{
		//Remember to change one of the motor direction according to data sheet since PWM cannot change direction.
		pwm_output(i,1300); // 0-100 (max speed), 50: 50% (?) of maximum speed = 1300 microseconds pulsewidth
		if (firing[i] == 0)
		{
			firing[i] = 1;
			start_time[i] = HAL_GetTick();
		}
		if (start_time[i] + 1000 < HAL_GetTick()) //If more than 1 s has passed since the fire command has been given,and flywheel has charged for 1s, feed
		{
			//since launcher has fired, might need to be cleared after firing stops
			last_fed_time = HAL_GetTick();
			if (fabs(feeders[i].torque) > FEEDER_JAM_TORQUE)
			{
				unjamming[i] = 1;
				jam_time[i] = HAL_GetTick();
			}

			//unjamming needed, check what feeder output to give
			if (unjamming[i] == 1)
			{
				// unjam time has elasped
				if (jam_time[i] + FEEDER_UNJAM_TIME < HAL_GetTick())
				{
					unjamming[i] = 0;
					feeder_output[i] = FEEDER_SPEED;
				}
				// feeder is still unjamming, hence, send unjam speed
				else
				{
					feeder_output[i] = FEEDER_UNJAM_SPD;
				}
			}
			else
			{
				feeder_output[i] = FEEDER_SPEED;
			}
			if (i == 0)
			{
				feeder_output[i] *= -1;
			}
			speed_pid(feeder_output[i] * 36,feeders[i].rpm, &feeders[i].pid);
		}
		else //If less than 1 s has passed since the fire command has been given,and flywheel has charged for less than 1s, do not feed
		{
			speed_pid(0,feeders[i].rpm, &feeders[i].pid);
		}
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

