/*
 * usartWrapper.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#include "usartWrapper.h"
#include "string.h"
#include "bsp.h"
#include "misc.h"
#include "dbg_base.h"

#if 0
#include "dbg_trace.h"
#endif

static USART_HandleTypeDef *s_deviceHandle = NULL;

HAL_StatusTypeDef USART2_Init(USART_HandleTypeDef *handle, uint32_t baudRate) {

	HAL_StatusTypeDef result = HAL_ERROR;
	USART_InitTypeDef ifaceParams = {
			baudRate,
			USART_WORDLENGTH_8B,
			USART_STOPBITS_1,
			USART_PARITY_NONE,
			USART_MODE_TX_RX,
			USART_POLARITY_LOW,
			0,0
	};
	if (handle) {
		s_deviceHandle = handle;
		memset(handle, 0, sizeof(*handle));
		handle->Instance = USART2;
		handle->Init = ifaceParams;

		result = HAL_USART_Init(handle);
		HAL_NVIC_EnableIRQ(USART2_IRQn);
	}
	return result;
}

void USART_handleEvent(Event_p event) {
	if (!event)
		return;
	USART_HandleTypeDef *husart = event->data.uxart.hUsart;
	int i;
	switch (event->subType.uxart) {
		case ES_UxART_RX:
			DBGMSG_M("[RX] id [%d] state %p, %d",
					HELP_getUartIdByHandle(husart), HAL_USART_GetState(husart), husart->RxXferSize);
			for(i = 0; i < husart->RxXferSize; i++) {
				DBGMSG_L(" %c", husart->pRxBuffPtr[i]);
			}
			break;
		case ES_UxART_TX:
			DBGMSG_M("[TX] id [%d] state %p size %d",
					HELP_getUartIdByHandle(husart), HAL_USART_GetState(husart), husart->TxXferSize);
			break;
		case ES_UxART_RXTX:
			DBGMSG_M("[RXTX] id [%d] state %p RXxTX %dx%d",
					HELP_getUartIdByHandle(husart), HAL_USART_GetState(husart), husart->RxXferSize, husart->TxXferSize);
			break;
		case ES_UxART_ERROR:
			DBGMSG_ERR("id [%d] state %p errno %p",
					HELP_getUartIdByHandle(husart), event->data.uxart.buffer, event->data.uxart.size);
			HELP_dumpUartError(event->data.uxart.size);
			break;
	}
}

void USART2_IRQHandler(void) {
	HAL_USART_IRQHandler(s_deviceHandle);
}

void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart) {
	Event_t event = { EVENT_USART, { ES_UxART_RX },
			.data.uxart.hUsart = husart
	};
	BSP_queuePush(&event);
}
void HAL_USART_TxCpltCallback(USART_HandleTypeDef *husart) {
	if (!Trace_onTxComplete(husart)) {
		Event_t event = { EVENT_USART, { ES_UxART_TX },
				.data.uxart.hUsart = husart
		};
		BSP_queuePush(&event);
	}
}
void HAL_USART_TxRxCpltCallback(USART_HandleTypeDef *husart) {
	Event_t event = { EVENT_USART, { ES_UxART_RXTX },
			.data.uxart.hUsart = husart
	};
	BSP_queuePush(&event);
}
void HAL_USART_ErrorCallback(USART_HandleTypeDef *husart) {
	Event_t event = { EVENT_USART, { ES_UxART_ERROR },
			.data.uxart.hUsart = husart
	};
	BSP_queuePush(&event);
}
