/*
 * actuator_feedback_task.h
 *
 *  Created on: 19 Jan 2021
 *      Author: Hans Kurnia
 */

#ifndef TASKS_INC_ACTUATOR_FEEDBACK_H_
#define TASKS_INC_ACTUATOR_FEEDBACK_H_



void convert_raw_can_data(uint16_t motor_id, uint8_t *rx_buffer);
void actuator_feedback_task(void *argument);
void gimbal_offset(gimbal_data_t *gimbal_data);

#endif /* TASKS_INC_ACTUATOR_FEEDBACK_H_ */
