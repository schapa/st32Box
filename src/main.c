//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "usartWrapper.h"
#include "canWrapper.h"
#include "pwmWrapper.h"
#include "trigger.h"
#include "misc.h"
#include "bsp.h"

#include "systemStatus.h"

int main(int argc, char* argv[]) {

	(void)argc;
	(void)argv;

	USART_HandleTypeDef traceUsart;
	CAN_HandleTypeDef can1Bus;
	HAL_StatusTypeDef initResult = HAL_ERROR;

	BSP_queueInit();
	SystemStatus_set(INFORM_IDLE);
	SystemStatus_setLedControl(Led_Green_SetState);

	USART_tracerInit(&traceUsart);
	initResult = CAN_init(&can1Bus);
	InitTrigger(&can1Bus);

	HELP_printMessage();
	HELP_dumpUsartProps(&traceUsart);
	HELP_dumpCANProps(&can1Bus);
	if (initResult != HAL_OK)
		trace_printf("Can init failed [%d] with code [%d]\n\r", initResult, can1Bus.ErrorCode);

	PWM_Init();
extern void USB_GenericInit(void);
USB_GenericInit();
//simpleTest();
extern void QueryTest(void);
QueryTest();
char buffer[50];
	while (true) {
		Event_t event;
		BSP_queueWaitForEvent(&event);
		switch (event.type) {
			case EVENT_EXTI:
				trace_printf("[Exti] pin %d act %d\n\r", event.data, event.subType.exti);
				break;
			case EVENT_SYSTICK:
				switch (event.subType.systick) {
					case ES_SYSTICK_TICK:
						trace_printf("[Uptime] %d ticks\n\r", event.data);
						break;
					case ES_SYSTICK_SECOND_ELAPSED:
						trace_printf("[Uptime] %d seconds\n\r", event.data);
						break;
				}
				break;
			case EVENT_USART: {
				USART_HandleTypeDef *husart = (USART_HandleTypeDef*)event.data;
				switch (event.subType.usart) {
					case ES_UsART_RX:
						trace_printf("[UART_RX] %p state %d\n\r", husart, husart->State);
						trace_printf("\t rx %d from %d\n\r", husart->RxXferCount, husart->RxXferSize);
						break;
					case ES_UsART_TX:
						trace_printf("[UART_TX] %p state %d\n\r", husart, husart->State);
						trace_printf("\t tx %d from %d\n\r", husart->TxXferCount, husart->TxXferSize);
						break;
					case ES_UsART_RXTX:
						trace_printf("[UART_RXTX] %p state %d\n\r", husart, husart->State);
						trace_printf("\t tx %d from %d\n\r", husart->TxXferCount, husart->TxXferSize);
						trace_printf("\t rx %d from %d\n\r", husart->RxXferCount, husart->RxXferSize);
						break;
					case ES_UsART_ERROR:
						trace_printf("[UART_ERROR] %p errno %d\n\r", husart, husart->ErrorCode);
						break;
				}
			}
			default:
				trace_printf("Unhandled Event type %p data %p!\n\r", event.type, event.data);
				break;
		}
	}

	return 0;
}
