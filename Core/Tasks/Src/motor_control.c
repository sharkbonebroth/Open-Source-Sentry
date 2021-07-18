/*
 * pid.c
 *
 *  Created on: May 23, 2021
 *      Author: wx
 */

#include "board_lib.h"
#include "motor_control.h"
#include "robot_config.h"


//todo: clean this place

void speed_pid(double setpoint, double curr_pt, pid_data_t *pid)
{
	double Pout = 0;
	double Iout = 0;
	double Dout = 0;
	pid->error[2] = pid->error[1];
	pid->error[1] = pid->error[0];
	pid->error[0] = setpoint - curr_pt;
	Pout = pid->error[0] * pid->kp;

    pid->error_buf[2] = pid->error_buf[1];
    pid->error_buf[1] = pid->error_buf[0];
    pid->error_buf[0] = pid->error[0] - pid->error[1];
    Dout = pid->error_buf[0] * pid->kd;

    Iout = pid->error[0] * pid->ki;
    pid->output += Pout + Iout + Dout;
//    double_limit(&Iout, pid->int_max);
    int_limit(&pid->output, pid->max_out);
}


void angle_pid(double setpoint, double curr_pt, gimbal_data_t *motor)
{
	float ang_diff = (setpoint - curr_pt);
	if (ang_diff > PI )
	{
		ang_diff -= 2 * PI;
	}
	else if(ang_diff < -PI)
	{
		ang_diff += 2 * PI;
	}

	int16_t set_rpm = (ang_diff) * motor->rpm_max / GIMBAL_MAX_ANGLE_DIFF;
	speed_pid(set_rpm, motor->rpm, &motor->pid);


}



/**
 * CAN command function for CAN1 only
 * @param motor1: data to be put in the first 2 bytes of the CAN messagee
 * @param motor2: data to be put in the next 2 bytes
 * @param motor3: 5th and 6th byte
 * @param motor4: 7th and 8th byte
 * @param can_stdID: the header ID, i.e. 0x200, 0x1FF and 0x2FF
 *
 * @note: this function probably only works for DJI motors due to the 2 bytes each motors require
 */
void CANone_cmd(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4, uint32_t can_stdID)
{
	CAN_TxHeaderTypeDef  CANone_tx_message;
	uint8_t              CANone_send_data[8];
	uint32_t send_mail_box;
	CANone_tx_message.StdId = can_stdID;
	CANone_tx_message.IDE = CAN_ID_STD;
	CANone_tx_message.RTR = CAN_RTR_DATA;
	CANone_tx_message.DLC = 0x08;
	CANone_send_data[0] = motor1 >> 8;
	CANone_send_data[1] = motor1;
	CANone_send_data[2] = motor2 >> 8;
	CANone_send_data[3] = motor2;
	CANone_send_data[4] = motor3 >> 8;
	CANone_send_data[5] = motor3;
	CANone_send_data[6] = motor4 >> 8;
	CANone_send_data[7] = motor4;
	HAL_CAN_AddTxMessage(&hcan1, &CANone_tx_message, CANone_send_data, &send_mail_box);
}


/**
 * CAN command function for CAN2 only
 * @param motor1: data to be put in the first 2 bytes of the CAN messagee
 * @param motor2: data to be put in the next 2 bytes
 * @param motor3: 5th and 6th byte
 * @param motor4: 7th and 8th byte
 * @param can_stdID: the header ID, i.e. 0x200, 0x1FF and 0x2FF
 *
 * @note: this function probably only works for DJI motors due to the 2 bytes each motors require
 */
void CANtwo_cmd(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4, uint32_t can_stdID)
{
	static CAN_TxHeaderTypeDef  CANtwo_tx_message;
	static uint8_t              CANtwo_send_data[8];
	uint32_t send_mail_box;
	CANtwo_tx_message.StdId = can_stdID;
	CANtwo_tx_message.IDE = CAN_ID_STD;
	CANtwo_tx_message.RTR = CAN_RTR_DATA;
	CANtwo_tx_message.DLC = 0x08;
	CANtwo_send_data[0] = motor1 >> 8;
	CANtwo_send_data[1] = motor1;
	CANtwo_send_data[2] = motor2 >> 8;
	CANtwo_send_data[3] = motor2;
	CANtwo_send_data[4] = motor3 >> 8;
	CANtwo_send_data[5] = motor3;
	CANtwo_send_data[6] = motor4 >> 8;
	CANtwo_send_data[7] = motor4;
	HAL_CAN_AddTxMessage(&hcan2, &CANtwo_tx_message, CANtwo_send_data, &send_mail_box);
}


/**
 * Limits the integral inputs so it doesn't go to crazy amounts like 100k
 *
 * @params integral: pointer to the integral variable
 * @params integral_max: the positive max of the integral value
 *
 */
void ki_limit(float *integral, float integral_max)
{
	if (*integral > integral_max)
	{
		*integral = integral_max;
	}
	else if (*integral < -integral_max)
	{
		*integral = -integral_max;
	}
}


/**
 * Limits the input int16 variable
 * @params motor_in: the pointer to the variable to be limited
 * @params motor_max: the positive maximum value for the variable
 */

void motor_minmax(int16_t *motor_in, int16_t motor_max, int16_t motor_min)
{
	if (*motor_in > motor_max)
	{
		*motor_in = motor_max;
	}
	else if (*motor_in < -motor_max)
	{
		*motor_in = -motor_max;
	}

	if (fabs(*motor_in) < motor_min)
	{
		*motor_in = 0;
	}

}


void motor_limit(int16_t *motor_in, int16_t motor_max)
{
	if (*motor_in > motor_max)
	{
		*motor_in = motor_max;
	}
	else if (*motor_in < -motor_max)
	{
		*motor_in = -motor_max;
	}

}

void int_limit(int16_t *vIn, int16_t max)
{
	if (*vIn > max)
	{
		*vIn = max;
	}
	else if (*vIn < -max)
	{
		*vIn = -max;
	}
}




void double_limit(double *vIn, double max)
{
	if (*vIn > max)
	{
		*vIn = max;
	}
	else if (*vIn < -max)
	{
		*vIn = -max;
	}
}

/**
 * Resets PID values for the motors using the motor_data_t struct
 * @params motor_data: pointer to the motor data struct
 *
 * todo: add in one to reset gimbal struct
 */
void reset_pid(motor_data_t *motor_data)
{
	motor_data->pid.integral = 0;
	motor_data->pid.error[0] = 0;
	motor_data->pid.error[1] = 0;
	motor_data->pid.error[2] = 0;
	motor_data->output = 0;
}

