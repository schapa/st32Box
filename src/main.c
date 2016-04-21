/*
 * main.c
 *
 *  Created on: Apr 21, 2016
 *      Author: shapa
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"

#include "bsp.h"
#include "USB_Generic.h"
#include "misc.h"

int main(int argc, char* argv[]) {

	(void)argc;
	(void)argv;

	BSP_init();
//simpleTest();
extern void QueryTest(void);
QueryTest();
	while (true) {
		Event_t event;
		BSP_queuePendEvent(&event);
		switch (event.type) {
			case EVENT_EXTI:
				trace_printf("[Exti] pin %d act %d\n\r", event.data, event.subType.exti);
				char *text = "hello!";
				USB_ACM_write((uint8_t*)text, sizeof(text));
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
				USART_HandleTypeDef *husart = event.data.usart.hUsart;
				switch (event.subType.uxart) {
					case ES_UsART_RX:
						trace_printf("[USART_RX] id [%d] state %d\n\r",
								HELP_getUsartIdByHandle(husart), HAL_USART_GetState(husart));
						trace_printf("\t rx %d from %d\n\r", husart->RxXferCount, husart->RxXferSize);
						break;
					case ES_UsART_TX:
						trace_printf("[USART_TX] id [%d] state %d\n\r",
								HELP_getUsartIdByHandle(husart), HAL_USART_GetState(husart));
						trace_printf("\t tx %d from %d\n\r", husart->TxXferCount, husart->TxXferSize);
						break;
					case ES_UsART_RXTX:
						trace_printf("[USART_RXTX] id [%d] state %d\n\r",
								HELP_getUsartIdByHandle(husart), HAL_USART_GetState(husart));
						trace_printf("\t tx %d from %d\n\r", husart->TxXferCount, husart->TxXferSize);
						trace_printf("\t rx %d from %d\n\r", husart->RxXferCount, husart->RxXferSize);
						break;
					case ES_UsART_ERROR:
						trace_printf("[USART_ERROR] id [%d] state %d errno %d\n\r",
								HELP_getUsartIdByHandle(husart), HAL_USART_GetState(husart), HAL_USART_GetError(husart));
						break;
				}
			} break;
			case EVENT_CAN: {
				CAN_HandleTypeDef *hcan = event.data.can.hCan;
				switch (event.subType.can) {
					case ES_CAN_RX: {
						CanRxMsgTypeDef *rx = &event.data.can.rxMsg;
						HELP_dumpCANRxMsg(rx);
						} break;
					case ES_CAN_TX: {
						CanTxMsgTypeDef *tx = &event.data.can.txMsg;
						trace_printf("[CAN] tx [%p] ok\n\r", tx->IDE ? tx->ExtId : tx->StdId);
						} break;
					case ES_CAN_ERROR:
						trace_printf("[CAN_ERROR] id [%d] state %d errno %d\n\r",
								HELP_getCanIdByHandle(hcan), HAL_CAN_GetState(hcan), HAL_CAN_GetError(hcan));
						break;
				}
			} break;
			default:
				trace_printf("Unhandled Event type %p data %p!\n\r", event.type, event.data);
				break;
		}
	}

	return 0;
}
