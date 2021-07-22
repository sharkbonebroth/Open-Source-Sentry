/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

#include "board_lib.h"
#include "startup_task.h"
#include "gimbal_control_task.h"
#include "movement_control_task.h"
#include "gun_control_task.h"
#include "robot_config.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


//Thread attributes, Low <-> High
const osThreadAttr_t low_priority_task_attributes = {
		.name = "low_priority_task",
		.priority = (osPriority_t) osPriorityLow
};

const osThreadAttr_t low2_priority_task_attributes = {
		.name = "low2_priority_task",
		.priority = (osPriority_t) osPriorityLow2
};

const osThreadAttr_t high_priority_task_attributes = {
    .name = "high_priority_task",
    .priority = (osPriority_t) osPriorityHigh
};

const osThreadAttr_t high2_priority_task_attributes = {
    .name = "high2_priority_task",
    .priority = (osPriority_t) osPriorityHigh2
};


/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
//FreeRTOS definitions
#define ISR_SEMAPHORE_COUNT 1
#define QUEUE_SIZE 1

osSemaphoreId_t sem_refUart;
osEventFlagsId_t gimbal_data_flag;
osEventFlagsId_t chassis_data_flag;
osEventFlagsId_t gun_data_flag;
osEventFlagsId_t rc_data_flag;
osEventFlagsId_t control_data_flag;

osMessageQueueId_t m_gimbal_heading;
osMessageQueueId_t m_chassis_output;
osMessageQueueId_t m_gun_output;
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

//Input control task definitions
osThreadId_t gimbal_control_task_handle;
osThreadId_t movement_control_task_handle;
osThreadId_t gun_control_task_handle;

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .priority = (osPriority_t) osPriorityNormal,
  .stack_size = 128 * 4
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
//	startup_task();
	gimbal_data_flag = osEventFlagsNew(NULL);
	chassis_data_flag = osEventFlagsNew(NULL);
	gun_data_flag = osEventFlagsNew(NULL);
	rc_data_flag = osEventFlagsNew(NULL);
	control_data_flag = osEventFlagsNew(NULL);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	sem_refUart = osSemaphoreNew(ISR_SEMAPHORE_COUNT, 0, NULL);

	//Controls when different task can execute

  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  //todo: adjust priorities
  //Threads creation
  	gimbal_control_task_handle   = osThreadNew(gimbal_control_task, NULL, &high2_priority_task_attributes);
  	movement_control_task_handle = osThreadNew(movement_control_task, NULL, &high_priority_task_attributes);//run now
  	gun_control_task_handle = osThreadNew(gun_control_task, NULL, &high_priority_task_attributes);
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  	//todo: add event flags
	//Signals when a certain event has occurred

  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
