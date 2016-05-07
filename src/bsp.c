/*
 * bsp.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */


#include <stdbool.h>
#include <system.h>

#include "bsp.h"
#include "stm32f4xx_hal.h"
#include "misc.h"
#include "uartWrapper.h"
#include "usartWrapper.h"
#include "extiWrapper.h"
#include "canWrapper.h"
#include "pwmWrapper.h"
#include "USB_Generic.h"
#include "dbg_base.h"
#include "Engine.h"

#if 0
#include "dbg_trace.h"
#endif

static void initGPIO_LED(void);

static USART_HandleTypeDef s_traceUsart;
static UART_HandleTypeDef s_espUart;
static DMA_HandleTypeDef s_traceTxDma;
static CAN_HandleTypeDef s_can1Bus;

void BSP_init(void) {

	USART_HandleTypeDef *pTraceUsart = &s_traceUsart;
	DMA_HandleTypeDef *pTraceTxDma = &s_traceTxDma;
	CAN_HandleTypeDef *pCan1Bus = &s_can1Bus;
	HAL_StatusTypeDef initResult = HAL_OK;
	initGPIO_LED();

	initResult |= Trace_InitUSART1(pTraceUsart, pTraceTxDma);
	initResult |= UART4_Init(&s_espUart, 115200);
	initResult |= CAN_init(pCan1Bus);

	HELP_printMessage();
	HELP_dumpUsartProps(pTraceUsart);
	HELP_dumpCANProps(pCan1Bus);

	System_setStatus(INFORM_IDLE);
	System_setLedControl(Led_Green_SetState);

	EXTI_baseInit();
	PWM_Init();
	USB_ACM_devInit();

	if (initResult != HAL_OK) {
		DBGMSG_ERR("Init failed\n\r" \
				"\t [USART_%d] state %d errno %d\n\r",
				"\t [CAN_%d] state %d errno %d\n\r",
				HELP_getUsartIdByHandle(pTraceUsart), HAL_USART_GetState(pTraceUsart), HAL_USART_GetError(pTraceUsart),
				0, HAL_CAN_GetState(pCan1Bus), HAL_CAN_GetError(pCan1Bus));
	}
}

void BSP_queuePush(Event_p pEvent) {
	SystemEvent *evt = Q_NEW(SystemEvent, SYSTEM_SIG);
	evt->event = *pEvent;
    QACTIVE_POST(AO_system(), evt, NULL);
}

void BSP_espSend(const char *buff, size_t size) {
	HAL_UART_Transmit_IT(&s_espUart, buff, size);
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
