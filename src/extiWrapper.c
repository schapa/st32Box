/*
 * trigger.c
 *
 *  Created on: Mar 27, 2016
 *      Author: shapa
 */

#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "extiWrapper.h"
#include "bsp.h"

void EXTI_baseInit(void) {

	GPIO_InitTypeDef iface = {
			GPIO_PIN_0,
			GPIO_MODE_IT_FALLING,
			GPIO_NOPULL,
			GPIO_SPEED_FREQ_LOW,
			0
	};
	__HAL_RCC_GPIOA_CLK_ENABLE();
	HAL_GPIO_Init(GPIOA, &iface);
	HAL_NVIC_EnableIRQ(EXTI0_IRQn);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
	Event_t event = { EVENT_EXTI, { ES_EXTI_DOWN }, { GPIO_Pin } };
	BSP_queuePush(&event);
}

void EXTI0_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_0);
}
void EXTI1_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_1);
}
void EXTI2_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_2);
}
void EXTI3_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_3);
}
void EXTI4_IRQHandler(void) {
	HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_4);
}
