/*
 * bsp.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */


#include <stdbool.h>

#include "bsp.h"
#include "stm32f4xx_hal.h"
#include "misc.h"
#include "usartWrapper.h"
#include "systemStatus.h"
#include "extiWrapper.h"
#include "canWrapper.h"
#include "pwmWrapper.h"
#include "USB_Generic.h"

static void initGPIO_LED(void);
static void initQueue(void);

static EventQueue_p s_eventQueue;
static USART_HandleTypeDef s_traceUsart;
static CAN_HandleTypeDef s_can1Bus;

void BSP_init(void) {

	USART_HandleTypeDef *pTraceUsart = &s_traceUsart;
	CAN_HandleTypeDef *pCan1Bus = &s_can1Bus;
	HAL_StatusTypeDef initResult = HAL_OK;
	initGPIO_LED();
	initQueue();

	initResult &= USART_tracerInit(pTraceUsart);
	initResult &= CAN_init(pCan1Bus);

	HELP_printMessage();
	HELP_dumpUsartProps(pTraceUsart);
	HELP_dumpCANProps(pCan1Bus);

	SystemStatus_set(INFORM_IDLE);
	SystemStatus_setLedControl(Led_Green_SetState);

	EXTI_baseInit();
	PWM_Init();
	USB_ACM_devInit();

	if (initResult != HAL_OK) {
		trace_printf("Init failed\n\r" \
				"\t [USART_%d] state %d errno %d\n\r",
				"\t [CAN_%d] state %d errno %d\n\r",
				HELP_getUsartIdByHandle(pTraceUsart), HAL_USART_GetState(pTraceUsart), HAL_USART_GetError(pTraceUsart),
				0, HAL_CAN_GetState(pCan1Bus), HAL_CAN_GetError(pCan1Bus));
	}
}

void BSP_queuePush(Event_p pEvent) {
	Queue_pushEvent(s_eventQueue, pEvent);
}

void BSP_queuePendEvent(Event_p pEvent) {
	Queue_getEvent(s_eventQueue, pEvent, true);
}

_Bool BSP_queueIsEventPending(Event_p pEvent) {
	return Queue_getEvent(s_eventQueue, pEvent, false);
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

static void initQueue(void) {
	static _Bool init = false;
	if (!init) {
		Queue_new(&s_eventQueue);
		init = true;
	}
}
