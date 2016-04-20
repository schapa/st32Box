/*
 * bsp.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#include "bsp.h"
#include <stdbool.h>

#include "stm32f4xx_hal.h"

static void initGPIO_LED(void);

static EventQueue_p s_eventQueue;

void BSP_init(void) {
	initGPIO_LED();
}

void BSP_queueInit(void) {
	static _Bool init = false;
	if (!init) {
		Queue_new(&s_eventQueue);
		init = true;
	}
}

void BSP_queuePush(Event_p pEvent) {
	Queue_pushEvent(s_eventQueue, pEvent);
}

void BSP_queueWaitForEvent(Event_p pEvent) {
	Queue_getEvent(s_eventQueue, pEvent, true);
}

void Led_Red_SetState(FunctionalState state) {
	GPIO_PinState val = (state == DISABLE) ? GPIO_PIN_RESET : GPIO_PIN_SET;
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_14, val);
}

void Led_Green_SetState(FunctionalState state) {
	GPIO_PinState val = (state == DISABLE) ? GPIO_PIN_RESET : GPIO_PIN_SET;
	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, val);
}

static void initGPIO_LED(void) {
	__HAL_RCC_GPIOG_CLK_ENABLE();
	GPIO_InitTypeDef iface = {
			GPIO_PIN_13 | GPIO_PIN_14,
			GPIO_MODE_OUTPUT_PP,
			GPIO_NOPULL,
			GPIO_SPEED_FREQ_LOW,
			0
	};
	HAL_GPIO_Init(GPIOG, &iface);
}
