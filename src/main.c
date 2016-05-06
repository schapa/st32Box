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
#include "stm32f4xx_hal.h"

#include "bsp.h"
#include "USB_Generic.h"
#include "uartWrapper.h"
#include "canWrapper.h"
#include "misc.h"
#include "timers.h"
#include "dbg_base.h"
#include "Query.h"

#if 01
#include "dbg_trace.h"
#endif

int main(int argc, char* argv[]) {
	DBG_ENTRY;
	(void)argc;
	(void)argv;

	BSP_init();

extern void QueryTest(uint8_t *buff, size_t size);
QueryTest(NULL, 0);
	while (true) {
		Event_t event;
		BSP_pendEvent(&event);
		switch (event.type) {
			case EVENT_EXTI: {
				DBGMSG_L("[Exti] pin %d act %d", event.data, event.subType.exti);
				char *text = "hello !";
				CanTxMsgTypeDef txMsg = {
						0x22, 0,
						CAN_ID_STD,
						CAN_RTR_DATA,
						8,
						{ text[0], text[1], text[2], text[3], text[4], text[5], text[6], text[7] }
				};
				USB_ACM_write((uint8_t*)text, strlen(text));
				CAN_write(&txMsg);
				} break;
			case EVENT_UART:
				UART_handleEvent(&event);
				break;
			case EVENT_UxART_Buffer: {
				uint8_t *buff = (uint8_t *)event.data.uxart.buffer;
				size_t size = event.data.uxart.size;
				switch (event.subType.uxart) {
					case ES_UxART_RX:
						DBGMSG_H("[RX] [%s]", buff);
						QueryTest(buff, size);
						MEMMAN_free(buff);
						break;
					default:
						break;
				}
			} break;
			case EVENT_CAN:
				CAN_handleEvent(&event);
				break;
			case EVENT_SYSTICK:
				if (event.subType.systick == ES_SYSTICK_SECOND_ELAPSED) {
					Timer_makeTick();
				}
			case EVENT_USART:
				USART_handleEvent(&event);
				break;
			case EVENT_DUMMY:
				break;
			default:
				DBGMSG_WARN("Unhandled Event type %p data %p!", event.type, event.data);
				break;
		}
	}
	DBG_EXIT;
	return 0;
}
