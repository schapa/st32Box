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
static void initGPIO_PWM(void);
static void initGPIO_USB(void);

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

void HAL_TIM_PWM_MspInit(TIM_HandleTypeDef *htim) {
	if (htim->Instance == TIM1) {
		__HAL_RCC_GPIOA_CLK_ENABLE();
		__HAL_RCC_TIM1_CLK_ENABLE();
		initGPIO_PWM();
	}
}

void HAL_PCD_MspInit(PCD_HandleTypeDef *hpcd) {
	if (hpcd->Instance == USB_OTG_FS) {
		__HAL_RCC_USB_OTG_FS_CLK_ENABLE();
	} else if (hpcd->Instance == USB_OTG_HS) {
		__HAL_RCC_USB_OTG_HS_CLK_ENABLE();
	}
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	initGPIO_USB();
}

void HAL_PCD_MspDeInit(PCD_HandleTypeDef *hpcd) {
	if (hpcd->Instance == USB_OTG_FS) {
		__HAL_RCC_USB_OTG_FS_CLK_DISABLE();
	} else if (hpcd->Instance == USB_OTG_HS) {
		__HAL_RCC_USB_OTG_HS_CLK_DISABLE();
	} else {
		__HAL_RCC_USB_OTG_FS_CLK_DISABLE();
		__HAL_RCC_USB_OTG_HS_CLK_DISABLE();
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

static void initGPIO_USB(void) {
//	PB12 - USB_OTG_ID
//	PB13 - USB_OTG_VBUS
//	PB14 - USB_OTG_DM
//	PB15 - USB_OTG_DP
//	PC4 - USB_PSO
//	PC5 - USB_OC

	GPIO_InitTypeDef iface = {
			GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15,
			GPIO_MODE_AF_PP,
			GPIO_NOPULL,
			GPIO_SPEED_FREQ_VERY_HIGH,
			GPIO_AF12_OTG_HS_FS
	};
	GPIO_InitTypeDef ifacePSO = {
			GPIO_PIN_4,
			GPIO_MODE_OUTPUT_PP,
			GPIO_NOPULL,
			GPIO_SPEED_FREQ_LOW,
			0
	};
	GPIO_InitTypeDef ifaceOC = {
			GPIO_PIN_5,
			GPIO_MODE_INPUT,
			GPIO_NOPULL,
			GPIO_SPEED_FREQ_LOW,
			0
	};
	HAL_GPIO_Init(GPIOB, &iface);
	HAL_GPIO_Init(GPIOC, &ifacePSO);
	HAL_GPIO_Init(GPIOC, &ifaceOC);
}

static void initGPIO_PWM(void) {
	GPIO_InitTypeDef iface = {
			GPIO_PIN_8,
			GPIO_MODE_AF_PP,
			GPIO_NOPULL,
			GPIO_SPEED_FREQ_LOW,
			GPIO_AF1_TIM1
	};
	HAL_GPIO_Init(GPIOA, &iface);
}

