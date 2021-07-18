/*
 * robot_config.h
 *
 *  Created on: 19 Jan 2021
 *      Author: Hans Kurnia
 */

#ifndef TASKS_INC_ROBOT_CONFIG_H_
#define TASKS_INC_ROBOT_CONFIG_H_

/*
 * MOTOR IDS				PHYSICAL ID		ARRAY NO.
 * 0x201 -> FRONT RIGHT 	ID1				chassis_motor[0]
 * 0x202 -> FRONT LEFT		ID2				chassis_motor[1]
 * 0x203 -> BACK LEFT		ID3				chassis_motor[2]
 * 0x204 -> BACK RIGHT		ID4				chassis_motor[3]
 * 0x205 -> LEFT FRICITON	ID5				friction[0]
 * 0x206 -> RIGHT FRICTION	ID6				friction[1]
 * 0x207 -> M2006 FEEDER	ID7				feeder
 * 0x208 -> EMPTY
 * 0X209 -> PITCH			ID5				PITCH_MOTOR
 * 0X20A -> YAW				ID6				YAW_MOTOR
 */

/* !!!!!---------- Common configurations ----------!!!!!
 * Change these values to change values across the robot.
 */

/* Remote switch position settings
 * Top = 1
 * Middle = 3
 * Down = 2
 * can swap switch positions, but not name (without changing them in source)
 */



enum left_switch
{
	aimbot_enable = 1,
	launcher_off = 2,
	launcher_on = 3
};

enum right_switch
{
	all_off = 1,
	all_on = 2,
	gimbal_on = 3
};


#define KEY_OFFSET_W        ((uint16_t)0x01<<0)
#define KEY_OFFSET_S        ((uint16_t)0x01<<1)
#define KEY_OFFSET_A 		((uint16_t)0x01<<2)
#define KEY_OFFSET_D        ((uint16_t)0x01<<3)
#define KEY_OFFSET_Q        ((uint16_t)0x01<<4)
#define KEY_OFFSET_E        ((uint16_t)0x01<<5)
#define KEY_OFFSET_SHIFT    ((uint16_t)0x01<<6)
#define KEY_OFFSET_CTRL     ((uint16_t)0x01<<7)
#define KEYBD_MAX_SPD 440


//Launcher settings
#define FEEDER_KP 			0.10			// |
#define FEEDER_KI  			0				// | - FEEDER PID VALUES
#define FEEDER_KD  			6				// |
#define FEEDER_SPEED  		100				//	Feeder speed. Multiply by number of slots in the feeder to get how many projectiles per minute
#define FEEDER_JAM_TORQUE  	7000			//	Torque before feeder deemed to be jammed
#define FEEDER_MAX_CURRENT	8000			//  Max value to be sent to motors. M2006 max value can be 10000
#define FEEDER_UNJAM_SPD  	-50				//	Reverse unjam speed
#define FEEDER_UNJAM_TIME	500

#define FRICTION_KP  		0.1				// |
#define FRICTION_KD  		10				// | - FRICTION WHEELS PID VALUES
#define FRICTION_KI  		0				// |
#define FRICTION_MAX_CURRENT 16384			// |
#define FRICTION_INVERT		1				// | set to -1 to flip both friction wheel speeds
#define LAUNCHER_ID 		0x1FF

/* Robot level specific configuration
 * Values changes according to ROBOT_LEVEL.
 * Defaults to level 1 if invalid level given.
 */
#define ROBOT_LEVEL 3

#if ROBOT_LEVEL == 3

#define FRICTION_WHEEL_SPD 7000

#elif ROBOT_LEVEL == 2

#define FRICTION_WHEEL_SPD 4000

#else

#define FRICTION_WHEEL_SPD 1000

#endif


//Chassis settings
#define CHASSIS_KP  		0.5				// |
#define CHASSIS_KI  		0				// | - CHASSIS WHEELS PID VALUES
#define CHASSIS_KD  		50				// |

#define MAX_SPEED 			8000 			//Max speed of robot
#define TURNING_SPEED		4000 			//Rotation speed of robot
#define CHASSIS_CANOFFSET	(0x201)
#define CHASSIS_DELAY 		2
#define CHASSIS_MAX_CURRENT 15384
#define CHASSIS_MIN_CURRENT 1000
#define CHASSIS_CAN 		hcan1
#define CHASSIS_ID 			0x200

//Gimbal settings
#define YAW_INVERT  		-1				//1 to invert control -1 to disable
#define PITCH_INVERT  		-1				//1 to invert control -1 to disable

#define PITCH_KP	  		0.1				// |
#define PITCH_KD  			80				// | - GIMBAL PID VALUES
#define PITCH_KI  			0				// |
#define PITCH_SPEED 	 	0.01			//Speed of gimbal pitch control turning
#define PITCH_MAX_RPM		120				//Max pitch RPM
#define PITCH_CENTER		528
#define PITCH_MAX_CURRENT	20000

#define YAW_KP				0.1
#define YAW_KD				100
#define YAW_KI				0
#define YAW_SPEED 	 		0.01 			//Speed of gimbal yaw control turning
#define YAW_MAX_RPM			120				//Max yaw RPM
#define YAW_CENTER 			5387
#define YAW_MAX_CURRENT		20000

#define GM6020_MAX_DEFLECTION 	4096		//max value per side that motor sends back, 8192 / 2
#define GIMBAL_CAN 				hcan2
#define GIMBAL_MAX_ANGLE_DIFF	0.1			//angle before gimbal goes max RPM in radians
#define GIMBAL_MAX_INT			10
#define GIMBAL_ID 				0x2FF



//CAN IDs
#define MAX_RC_VALUE 		1320 //660 * 2

enum can_ids
{
	  ID_CHASSIS      	= 0x201,
	  ID_FEEDER_R      	= 0x205,
	  ID_FEEDER_L      	= 0x206,
	  ID_PITCH      	= 0x209,
	  ID_YAW        	= 0x20A
};

#define STARTUP_DELAY 		100

#define LOW_FREQ 			440
#define HIGH_FREQ			880
#define BUZZER_DELAY 		100
#define MOTOR_ONLINE_CHECK 	0

#define OBC_DATA_SIZE		8
#define HORIZONTAL_MULT 	1


#define REMOTE_TIMEOUT 500
#define XAVIER_TIMEOUT 50
#define XAVIER_YAW_MULT 0.1
#define XAVIER_PITCH_MULT 0.1
#endif /* TASKS_INC_ROBOT_CONFIG_H_ */
