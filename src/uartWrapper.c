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

#define BUFFER_SIZE 128

static UART_HandleTypeDef *s_Uart4 = NULL;

static void handleUsart4_RX(UART_HandleTypeDef *huart);
static _Bool isTerminal(char symb);
static size_t countValidChars(char *buffer, size_t size);

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
			trace_printf("[UART_RX] id [%d] state %p\n\r",
					HELP_getUartIdByHandle(huart), HAL_UART_GetState(huart));
			trace_printf("\t %d [", huart->RxXferSize);
			for(i = 0; i < huart->RxXferSize; i++) {
				trace_printf(" %c", huart->pRxBuffPtr[i]);
			}
			trace_printf("]\r\n");
			break;
		case ES_UxART_TX:
			trace_printf("[UART_TX] id [%d] state %p size %d\n\r",
					HELP_getUartIdByHandle(huart), HAL_UART_GetState(huart), huart->TxXferSize);
			huart->pTxBuffPtr -= huart->TxXferSize;
//			for(i = 0; i < huart->TxXferSize; i++) {
//				trace_printf(" %c", huart->pTxBuffPtr[i]);
//			}
			huart->TxXferSize = 0;
			break;
		case ES_UxART_RXTX:
			trace_printf("[UART_RXTX] id [%d] state %p RXxTX %dx%d\n\r",
					HELP_getUartIdByHandle(huart), HAL_UART_GetState(huart), huart->RxXferSize, huart->TxXferSize);
			break;
		case ES_UxART_ERROR:
			trace_printf("[UART_ERROR] id [%d] state %p errno %p\n\r",
					HELP_getUartIdByHandle(huart), event->data.uxart.buffer, event->data.uxart.size);
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
		handleUsart4_RX(huart);
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
	__HAL_UART_CLEAR_FLAG(huart,HAL_UART_GetError(huart));
	__HAL_UART_CLEAR_PEFLAG(huart);
	BSP_queuePush(&event);
}

static void handleUsart4_RX(UART_HandleTypeDef *huart) {
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
			char* newBuff = (intptr_t)MEMMAN_malloc(size+1);
			if (newBuff) {
				memcpy((void*)newBuff, buffer, size);
				newBuff[size] = 0;
				Event_t event = { EVENT_UxART_Buffer, { ES_UxART_RX },
					.data.uxart = { {huart}, newBuff, size }
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
