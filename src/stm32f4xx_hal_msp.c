/*
 * hal.c
 *
 *  Created on: Mar 20, 2016
 *      Author: pavelgamov
 */

#include "stm32f4xx_hal.h"
#include "bsp.h"

static void initGPIO_UARTtrace(void);
static void initGPIO_UARTclock(void);
static void initGPIO_CAN(void);

void HAL_MspInit(void) {
	LED_init();
}

void HAL_USART_MspInit(USART_HandleTypeDef *husart) {

	if (husart->Instance == USART1) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_USART1_CLK_ENABLE();
		initGPIO_UARTtrace();
	} else if (husart->Instance == USART2) {
		__HAL_RCC_GPIOD_CLK_ENABLE();
		__HAL_RCC_USART2_CLK_ENABLE();
		initGPIO_UARTclock();
	}
}

void HAL_CAN_MspInit(CAN_HandleTypeDef *hcan) {

	if (hcan->Instance == CAN1) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_CAN1_CLK_ENABLE();
		initGPIO_CAN();
	}
}

static void initGPIO_UARTtrace(void) {

	GPIO_InitTypeDef iface = {
			GPIO_PIN_9 | GPIO_PIN_10,
			GPIO_MODE_AF_PP,
			GPIO_NOPULL,
			GPIO_SPEED_FREQ_LOW,
			GPIO_AF7_USART1
	};
	HAL_GPIO_Init(GPIOA, &iface);
}

static void initGPIO_UARTclock(void) {

	GPIO_InitTypeDef iface = {
			GPIO_PIN_5,
			GPIO_MODE_AF_PP,
			GPIO_NOPULL,
			GPIO_SPEED_FREQ_LOW,
			GPIO_AF7_USART2
	};
	HAL_GPIO_Init(GPIOD, &iface);
}

static void initGPIO_CAN() {

	GPIO_InitTypeDef iface = {
			GPIO_PIN_11 | GPIO_PIN_12,
			GPIO_MODE_AF_PP,
			GPIO_NOPULL,
			GPIO_SPEED_FREQ_LOW,
			GPIO_AF9_CAN1
	};
	HAL_GPIO_Init(GPIOA, &iface);
}
