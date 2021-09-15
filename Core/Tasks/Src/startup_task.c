/*
 * calibration_task.c
 *
 *  Created on: 10 May 2021
 *      Author: wx
 */

/*
 * HELLHOLE TIME
 * calibration tasks
 * 1) initialise IMU
 * 2) check motor statuses (check until all values are set)?
 * 3) gimbal calibration**** (find the offset)
 * 4) set all the KP KI and KD values
 * 5) set all the max current values
 * 6) IMU calibration**** (find gimbal angle to acceleration readings)
 * 7) find appropriate chassis motor rpm vs yaw gimbal rpm
 * ratio for rotating hell
 * 8) SAVE ALL THE VALUES
 * 9) check if saved before, and escape if yes (well this goes on top
 * of course)
 * specific key combination for activating this calibration
 * THIS NEEDS TO BEEP AT YOU UNTIL IT'S SAFE TO CARRY ON
 */

#include "board_lib.h"
#include "robot_config.h"
#include "motor_control.h"
#include "actuator_feedback.h"
#include "startup_task.h"
#include "gimbal_control_task.h"
extern uint8_t imu_triggered;
extern remote_cmd_t remote_cmd;
extern can_data_t canone_data;

extern float pitch;
extern float yaw;


void startup_task()
{
	led_on();
	//general init
	can_start(&hcan1);
	can_start(&hcan2);
	buzzer_init();
	dbus_remote_start(remote_raw_data);
	HAL_Delay(STARTUP_DELAY);
	led_green_off();

	//PID values
	canone_data.CHASSIS.pid.max_out = CHASSIS_MAX_CURRENT;
	canone_data.CHASSIS.pid.min_out = CHASSIS_MIN_CURRENT;
	canone_data.CHASSIS.pid.kp = CHASSIS_KP;
	canone_data.CHASSIS.pid.ki = CHASSIS_KI;
	canone_data.CHASSIS.pid.kd = CHASSIS_KD;

	for (uint8_t i = 0; i < 2; i++){
		canone_data.FEEDER[i].pid.max_out = FRICTION_MAX_CURRENT;
		canone_data.FEEDER[i].pid.min_out = 0;
		canone_data.FEEDER[i].pid.kp 	= FRICTION_KP;
		canone_data.FEEDER[i].pid.ki 	= FRICTION_KI;
		canone_data.FEEDER[i].pid.kd 	= FRICTION_KD;
	}


	//gimbal PID input
	canone_data.pitch.pid.max_out		= PITCH_MAX_CURRENT;
	canone_data.pitch.pid.kp 			= PITCH_KP;
	canone_data.pitch.pid.kd			= PITCH_KD;
	canone_data.pitch.pid.ki			= PITCH_KI;
	canone_data.pitch.motor_max_deflection = GM6020_MAX_DEFLECTION;
	canone_data.pitch.calib 		= 1;
	canone_data.pitch.rpm_max 		= PITCH_MAX_RPM;
	canone_data.pitch.center_ang 	= PITCH_CENTER;
	canone_data.pitch.max_ang 		= PITCH_MAX_ANGLE;
	canone_data.pitch.min_ang		= PITCH_MIN_ANGLE;

	canone_data.yaw.pid.max_out			= YAW_MAX_CURRENT;
	canone_data.yaw.pid.kp 				= YAW_KP;
	canone_data.yaw.pid.kd				= YAW_KD;
	canone_data.yaw.pid.ki				= YAW_KI;
	canone_data.yaw.motor_max_deflection = GM6020_MAX_DEFLECTION;
	canone_data.yaw.center_ang		= YAW_CENTER;
	canone_data.yaw.calib 			= 1;
	canone_data.yaw.rpm_max 		= YAW_MAX_RPM;
	canone_data.yaw.max_ang 		= 0.8;
	canone_data.yaw.min_ang 		= -0.8;
	//motor online check, code does not continue if any fails
	if (MOTOR_ONLINE_CHECK == 1)
	{
		/*
		while (canone_data.CHASSIS.id == 0)
		{
			err_buzzer(1,1);
		}

		for (uint8_t i = 0; i < 2; i++){
			while (canone_data.FEEDER[i].id == 0)
			{
				err_buzzer(1,i + 2);
			}
		}
		*/

		/*
		//gimbal motors check
		while (canone_data.pitch.id == 0)
		{
			err_buzzer(1,8);
		}

		while (canone_data.yaw.id == 0)
		{
			err_buzzer(1,9);
		}
		*/
	}

	pitch = 0;
	yaw	  = 0;
	ok_buzzer(1,1);
	led_toggle();

}


void err_buzzer(uint8_t low, uint8_t high)
{
	for (int8_t i = 0; i < low; i ++)
	{
		//PWM
		buzzer(LOW_FREQ);
		HAL_Delay(BUZZER_DELAY);
		buzzer(0);
		HAL_Delay(BUZZER_DELAY);
	}
	for (int8_t i = 0; i < high; i ++)
	{
		buzzer(HIGH_FREQ);
		HAL_Delay(BUZZER_DELAY);
		buzzer(0);
		HAL_Delay(BUZZER_DELAY);
	}
}

void ok_buzzer(uint8_t high, uint8_t low){
	for (int8_t i = 0; i < high; i ++)
	{
		//PWM
		buzzer(HIGH_FREQ);
		HAL_Delay(BUZZER_DELAY);
		buzzer(0);
		HAL_Delay(BUZZER_DELAY);
	}
	for (int8_t i = 0; i < low; i ++)
	{
		buzzer(LOW_FREQ);
		HAL_Delay(BUZZER_DELAY);
		buzzer(0);
		HAL_Delay(BUZZER_DELAY);
	}
}

