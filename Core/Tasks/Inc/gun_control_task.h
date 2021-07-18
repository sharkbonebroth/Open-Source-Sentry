/*
 * gun_control_task.h
 *
 *  Created on: Jul 15, 2021
 *      Author: yanling
 */

#ifndef TASKS_INC_GUN_CONTROL_TASK_H_
#define TASKS_INC_GUN_CONTROL_TASK_H_

void gun_control_task(void *argument);
void launcher_control(motor_data_t *feeders);
void pwm_output(uint8_t timergroup, uint16_t output);
uint16_t cycle_to_pulse(uint8_t cycle);

#endif /* TASKS_INC_GUN_CONTROL_TASK_H_ */
