/*
 * motor_control.h
 *
 *  Created on: May 23, 2021
 *      Author: wx
 */

#ifndef TASKS_INC_MOTOR_CONTROL_H_
#define TASKS_INC_MOTOR_CONTROL_H_


void speed_pid(double setpoint, double curr_pt, pid_data_t *pid);
void angle_pid(double setpoint, double curr_pt, gimbal_data_t *motor);
void yaw_pid(double setpoint, double curr_pt, pid_data_t *pid);
void CANone_cmd(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4, uint32_t can_stdID);
void CANtwo_cmd(int16_t motor1, int16_t motor2, int16_t motor3, int16_t motor4, uint32_t can_stdID);
void motor_limit(int16_t *motor_in, int16_t motor_max);
void motor_minmax(int16_t *motor_in, int16_t motor_max, int16_t motor_min);
void reset_pid(motor_data_t *motor_data);
void ki_limit(float *integral, float integral_max);
void int_limit(int16_t *vIn, int16_t max);
void double_limit(double *vIn, double max);

#endif /* TASKS_INC_MOTOR_CONTROL_H_ */
