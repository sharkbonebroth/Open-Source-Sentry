/*
 * gimbal_control_task.h
 *
 *  Created on: 19 Jan 2021
 *      Author: Hans Kurnia
 */

#ifndef TASKS_INC_GIMBAL_CONTROL_TASK_H_
#define TASKS_INC_GIMBAL_CONTROL_TASK_H_

void xavier_ISR(DMA_HandleTypeDef *hdma);
void usart_ISR(UART_HandleTypeDef *UartHandle);

void gimbal_control_task(void *argument);
void gimbal_imu_control(gimbal_data_t *pitch_motor, gimbal_data_t *yaw_motor);
void gimbal_angle_control(gimbal_data_t *pitch_motor, gimbal_data_t *yaw_motor);
void gimbal_mouse_imu_control(gimbal_data_t *pitch_motor, gimbal_data_t *yaw_motor);
void yaw_sweep(gimbal_data_t *pitch_motor, gimbal_data_t *yaw_motor);

#endif /* TASKS_INC_GIMBAL_CONTROL_TASK_H_ */
