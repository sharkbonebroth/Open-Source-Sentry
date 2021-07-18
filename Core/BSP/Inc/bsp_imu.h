/**
 ***************************************(C) COPYRIGHT 2018 DJI***************************************
 * @file       bsp_imu.h
 * @brief      this file contains the common defines and functions prototypes for 
 *             the bsp_imu.c driver
 * @note         
 * @Version    V1.0.0
 * @Date       Jan-30-2018      
 ***************************************(C) COPYRIGHT 2018 DJI***************************************
 */

#ifndef __MPU_H__
#define __MPU_H__


#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "ist8310_reg.h"
#include "typedefs.h"
#include "mpu6500_reg.h"


#define MPU_DELAY(x) HAL_Delay(x)
extern mpu_data_t 		mpu_data;
extern imu_t      		imu;
extern gimbal_heading_t gimbal_heading;



void HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef *hspi);
uint8_t   imu_init(void);
void init_quaternion(void);
void mpu_get_data(void);
void imu_ahrs_update(void);
void imu_attitude_update(void);
void mpu_offset_call(void);

#endif


