/*
 * typedefs.h
 *
 *  Created on: May 23, 2021
 *      Author: wx
 */

#ifndef TASKS_INC_TYPEDEFS_H_
#define TASKS_INC_TYPEDEFS_H_

typedef struct
{
	float kp;
	float ki;
	float kd;
	double error[3];
	double integral;
	double error_buf[3];
	double int_max;
	int16_t max_out;
	int16_t min_out;
	int16_t output;
}pid_data_t;



typedef struct {
	uint16_t id;
	uint16_t angle;
	int16_t rpm;
	int16_t torque;
	uint8_t temp;
	int16_t output;
	pid_data_t pid;
} motor_data_t;


typedef struct {
	uint16_t id;
	uint16_t real_angle;
	float adj_ang;
	int16_t rpm;
	int16_t torque;
	uint8_t temp;
	int16_t output;
	int16_t max_out;
	uint16_t center_ang;
	uint16_t motor_max_deflection;
	float max_ang;
	float min_ang;
	uint8_t calib;
	int16_t rpm_max;
	uint16_t rpm_mult;
	pid_data_t pid;
} gimbal_data_t;



typedef struct {
	motor_data_t chassis_motors[4];
	motor_data_t leftfriction;
	motor_data_t rightfriction;
	motor_data_t feeder;
	gimbal_data_t pitch;
	gimbal_data_t yaw;
} can_data_t;


/* Struct containing cleaned data from remote */
typedef struct {
	/* Joysticks - Values range from -660 to 660 */
	int16_t right_x;
	int16_t right_y;
	int16_t left_x;
	int16_t left_y;
	/* Switches - Values range from 1 - 3 */
	int8_t left_switch;
	int8_t right_switch;
	/* Mouse movement - Values range from -32768 to 32767 */
	int16_t mouse_x;
	int16_t mouse_y;
	int16_t mouse_z;
	int32_t mouse_hori;
	int32_t mouse_vert;
	/* Mouse clicks - Values range from 0 to 1 */
	int8_t mouse_left;
	int8_t mouse_right;

	/* Keyboard keys mapping
	 * Bit0 -- W 键
	 * Bit1 -- S 键
	 *	Bit2 -- A 键
	 *	Bit3 -- D 键
	 *	Bit4 -- Q 键
	 *	Bit5 -- E 键
	 *	Bit6 -- Shift 键
	 *	Bit7 -- Ctrl 键
	 *
	 */
	int16_t keyboard_keys;
	uint32_t last_time;
} remote_cmd_t;




typedef struct
{
	int16_t ax;
	int16_t ay;
	int16_t az;

	int16_t mx;
	int16_t my;
	int16_t mz;

	int16_t temp;

	int16_t gx;
	int16_t gy;
	int16_t gz;

	int16_t ax_offset;
	int16_t ay_offset;
	int16_t az_offset;

	int16_t gx_offset;
	int16_t gy_offset;
	int16_t gz_offset;
} mpu_data_t;

typedef struct
{
	int16_t ax;
	int16_t ay;
	int16_t az;

	int16_t mx;
	int16_t my;
	int16_t mz;

	float temp;

	float wx; /*!< omiga, +- 2000dps => +-32768  so gx/16.384/57.3 =	rad/s */
	float wy;
	float wz;

	float vx;
	float vy;
	float vz;

	float rol;
	float pit;
	float yaw;
} imu_t;

typedef struct
{
	float pitch;
	float yaw;
	float roll;
}gimbal_heading_t;







//todo: do something with this maybe
/**
typedef struct
{
	uint8_t imu;
	uint8_t pitch;
	uint8_t yaw;
}; init_status_t;
*/

#endif /* TASKS_INC_TYPEDEFS_H_ */
