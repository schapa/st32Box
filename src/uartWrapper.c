/*
 * uartWrapper.c
 *
 *  Created on: Apr 22, 2016
 *      Author: shapa
 */

#include "uartWrapper.h"
#include "string.h"
#include "bsp.h"
#include "memman.h"
#include "misc.h"
#include "systemStatus.h"
#include "timers.h"
#include "dbg_base.h"

#if 0
#include "dbg_trace.h"
#endif

#define BUFFER_SIZE 128
#define FLUSH_TIMEOUT 4//(4*1000u) /* 4 seconds */

static UART_HandleTypeDef *s_Uart4 = NULL;
static uint32_t s_uart4BufferFlushTimer = INVALID_HANDLE;

static void handleUart4_RX(UART_HandleTypeDef *huart);
static _Bool isTerminal(char symb);
static size_t countValidChars(char *buffer, size_t size);
static void onBufferFlush(void *data);

HAL_StatusTypeDef UART4_Init(UART_HandleTypeDef *handle, uint32_t baudRate) {

	HAL_StatusTypeDef result = HAL_ERROR;

	UART_InitTypeDef ifaceParams = {
			baudRate,
			UART_WORDLENGTH_8B,
			UART_STOPBITS_1,
			UART_PARITY_NONE,
			UART_MODE_TX_RX,
			UART_HWCONTROL_NONE,
			DISABLE
	};
	if (handle) {
		s_Uart4 = handle;
		memset(handle, 0, sizeof(*handle));
		handle->Instance = UART4;
		handle->Init = ifaceParams;

		result = HAL_UART_Init(handle);
		HAL_NVIC_EnableIRQ(UART4_IRQn);
/* simulate acceptance */
		HAL_UART_RxCpltCallback(handle);
		s_uart4BufferFlushTimer = Timer_newArmed(FLUSH_TIMEOUT, true, onBufferFlush, handle);
	}
	return result;
}

void UART_handleEvent(Event_p event) {
	if (!event)
		return;
	UART_HandleTypeDef *huart = event->data.uxart.hUart;
	int i;
	switch (event->subType.uxart) {
		case ES_UxART_RX:
			DBGMSG_M("[RX] id [%d] state %p, %d",
					HELP_getUartIdByHandle(huart), HAL_UART_GetState(huart), huart->RxXferSize);
			for(i = 0; i < huart->RxXferSize; i++) {
				DBGMSG_L(" %c", huart->pRxBuffPtr[i]);
			}
			break;
		case ES_UxART_TX:
			DBGMSG_M("[TX] id [%d] state %p size %d",
					HELP_getUartIdByHandle(huart), HAL_UART_GetState(huart), huart->TxXferSize);
			huart->pTxBuffPtr -= huart->TxXferSize;
			huart->TxXferSize = 0;
			break;
		case ES_UxART_RXTX:
			DBGMSG_M("[RXTX] id [%d] state %p RXxTX %dx%d",
					HELP_getUartIdByHandle(huart), HAL_UART_GetState(huart), huart->RxXferSize, huart->TxXferSize);
			break;
		case ES_UxART_ERROR:
			DBGMSG_ERR("id [%d] state %p errno %p",
					HELP_getUartIdByHandle(huart), event->data.uxart.buffer, event->data.uxart.size);
			HELP_dumpUartError(event->data.uxart.size);
			if (huart == s_Uart4)
				HAL_UART_RxCpltCallback(huart);
			break;
	}
}

void UART4_IRQHandler(void) {
	HAL_UART_IRQHandler(s_Uart4);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	if (huart == s_Uart4) {
		handleUart4_RX(huart);
	} else {
		Event_t event = { EVENT_UART, { ES_UxART_RX },
			.data.uxart.hUart = huart
		};
		BSP_queuePush(&event);
	}
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	Event_t event = { EVENT_UART, { ES_UxART_TX },
			.data.uxart.hUart = huart
	};
	BSP_queuePush(&event);
}
void HAL_UART_TxRxCpltCallback(UART_HandleTypeDef *huart) {
	Event_t event = { EVENT_UART, { ES_UxART_RXTX },
			.data.uxart.hUart = huart
	};
	BSP_queuePush(&event);
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	Event_t event = { EVENT_UART, { ES_UxART_ERROR },
			.data.uxart.hUart = huart,
			.data.uxart.buffer = HAL_UART_GetState(huart),
			.data.uxart.size = HAL_UART_GetError(huart),
	};
	__HAL_UART_CLEAR_PEFLAG(huart);
	BSP_queuePush(&event);
	if (huart == s_Uart4) {
		handleUart4_RX(huart);
	}
}

static void handleUart4_RX(UART_HandleTypeDef *huart) {
	static char buffer[BUFFER_SIZE];
	static size_t currentPos;
	_Bool send = false;
	if (huart->RxXferSize) {
		if (++currentPos < BUFFER_SIZE) {
			if (isTerminal(buffer[currentPos-1])) {
				if (currentPos == 1) {
					currentPos = 0;
				} else if (countValidChars(buffer, currentPos)) {
					send = true;
					currentPos--;
				}
			}
		} else {
			send = true;
		}
		if (send && currentPos) {
			size_t size = currentPos * sizeof(char);
			char *newBuff = MEMMAN_malloc(size+1);
			if (newBuff) {
				Timer_rearm(s_uart4BufferFlushTimer);
				memcpy((void*)newBuff, buffer, size);
				newBuff[size] = 0;
				Event_t event = { EVENT_UxART_Buffer, { ES_UxART_RX },
					.data.uxart.hUart = huart,
					.data.uxart.buffer = (intptr_t)newBuff,
					.data.uxart.size = size
				};
				BSP_queuePush(&event);
			}
			currentPos = 0;
		}
	}
	HAL_UART_Receive_IT(huart, (uint8_t*)&buffer[currentPos], 1);
}

static _Bool isTerminal(char symb) {
	return (symb == '\n') || (symb == '\r');
}

static size_t countValidChars(char *buffer, size_t size) {
	size_t i;
	size_t sumbs = 0;
	if (buffer && size) {
		for (i = 0; i < size; i++) {
			if ((isTerminal(buffer[i])) || (buffer[i] == ' '))
				continue;
			sumbs++;
		}
	}
	return sumbs;
}

static void onBufferFlush(void *data) {
	DBG_ENTRY;
	UART_HandleTypeDef *handle = (UART_HandleTypeDef *)data;
	if (handle && handle->RxXferCount) {
		*handle->pRxBuffPtr = '\n';
		handle->RxXferCount = 0;
		if (handle->State == HAL_UART_STATE_BUSY_RX) {
			handle->State = HAL_UART_STATE_READY;
		} else if (handle->State == HAL_UART_STATE_BUSY_TX_RX) {
			handle->State = HAL_UART_STATE_BUSY_TX;
		}
		Timer_disarm(s_uart4BufferFlushTimer);
		HAL_UART_RxCpltCallback(handle);
	}
	DBG_EXIT;
}
