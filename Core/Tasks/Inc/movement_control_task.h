/*
 * movement_control_task.h
 *
 *  Created on: 19 Jan 2021
 *      Author: Zheng Hao
 */
#include "stdbool.h"

#ifndef TASKS_INC_MOVEMENT_CONTROL_TASK_H_
#define TASKS_INC_MOVEMENT_CONTROL_TASK_H_


void movement_control_task(void *argument);
void chassis_motion_control(motor_data_t *motor);
void chassis_sweep(motor_data_t *motor);
float update_current_position(bool homing);
float rpm_to_speed(float rpm);
void homing_sequence(motor_data_t *motor);
void generate_goal_position(void);



#endif /* TASKS_INC_MOVEMENT_CONTROL_TASK_H_ */
