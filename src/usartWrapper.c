/*
 * usartWrapper.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#include "usartWrapper.h"

#include "string.h"

#include "bsp.h"

static USART_HandleTypeDef *s_tracerHandle = NULL;
static USART_HandleTypeDef *s_deviceHandle = NULL;

HAL_StatusTypeDef USART1_InitTrace(USART_HandleTypeDef *handle) {

	HAL_StatusTypeDef result = HAL_ERROR;
	USART_InitTypeDef ifaceParams = {
			115200,
			USART_WORDLENGTH_8B,
			USART_STOPBITS_1,
			USART_PARITY_NONE,
			USART_MODE_TX_RX,
			0,0,0
	};
	if (handle) {
		s_tracerHandle = handle;
		memset(handle, 0, sizeof(*handle));
		handle->Instance = USART1;
		handle->Init = ifaceParams;

		result = HAL_USART_Init(handle);
	}
	return result;
}

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

int trace_write_usart(const char *buf, size_t nbyte) {
	if (s_tracerHandle) {
		HAL_USART_Transmit(s_tracerHandle, (uint8_t*)buf, nbyte, 0xFF);
	}
	return nbyte;
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
	Event_t event = { EVENT_USART, { ES_UxART_TX },
			.data.uxart.hUsart = husart
	};
	BSP_queuePush(&event);
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
