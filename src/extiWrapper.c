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
	Event_t event = { EVENT_EXTI, { ES_EXTI_DOWN }, {GPIO_Pin} };
	BSP_queuePush(&event);
	if (GPIO_Pin == GPIO_PIN_0) {
//		s_canHandle->pTxMsg = malloc(sizeof(*s_canHandle->pTxMsg));
//		memcpy(s_canHandle->pTxMsg->Data, "HELLO HJ", 8);
//		s_canHandle->pTxMsg->DLC = 8;
//		s_canHandle->pTxMsg->RTR = 0;
//		s_canHandle->pTxMsg->IDE = 0;
//		s_canHandle->pTxMsg->StdId = 0x22;
//		HAL_StatusTypeDef stat = HAL_CAN_Transmit_IT(s_canHandle);
//		trace_printf("Send [%d]\n\r", stat);

	}
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
