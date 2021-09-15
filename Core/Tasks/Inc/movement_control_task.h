/*
 * movement_control_task.h
 *
 *  Created on: 19 Jan 2021
 *      Author: Hans Kurnia
 */

#ifndef TASKS_INC_MOVEMENT_CONTROL_TASK_H_
#define TASKS_INC_MOVEMENT_CONTROL_TASK_H_


void movement_control_task(void *argument);
void chassis_motion_control();
void CAN_launcher_control(motor_data_t *left_friction_motor, motor_data_t *right_friction_motor, motor_data_t *feeder);
void chassis_keyboard_motion_control(motor_data_t *motor);
void chassis_motion_control_imu(motor_data_t *motor);
void chassis_sweep(motor_data_t *motor);
void update_current_position(void);

float current_position;
uint16_t prev_chassis_motor_angle;

#endif /* TASKS_INC_MOVEMENT_CONTROL_TASK_H_ */
