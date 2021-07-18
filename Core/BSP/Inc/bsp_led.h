/*
 * bsp_led.h
 *
 *  Created on: Jan 19, 2020
 *      Author: Kai Yang
 */

#ifndef INC_BSP_LED_H_
#define INC_BSP_LED_H_

#include "stm32f4xx_hal.h"
#include "gpio.h"

#define LED_RED_Pin GPIO_PIN_11
#define LED_RED_GPIO_Port GPIOE
#define LED_GREEN_Pin GPIO_PIN_14
#define LED_GREEN_GPIO_Port GPIOF

/*
 * Controls green led. ON | OFF | Toggle
 */
void led_green_on(void);
void led_green_off(void);
void led_green_toggle(void);

/*
 * Controls red led. ON | OFF | Toggle
 */
void led_red_on(void);
void led_red_off(void);
void led_red_toggle(void);

/*
 * Controls both led. BOTH ON | BOTH OFF | TOGGLE TOGETHER
 */
void led_off(void);
void led_on(void);
void led_toggle(void);

#endif /* INC_BSP_LED_H_ */
