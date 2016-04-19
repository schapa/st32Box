/*
 * usartWrapper.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#include "usartWrapper.h"
#include "string.h"

static USART_HandleTypeDef *s_tracerHandle = NULL;
static USART_HandleTypeDef *s_deviceHandle = NULL;

HAL_StatusTypeDef USART_tracerInit(USART_HandleTypeDef *handle) {

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

HAL_StatusTypeDef USART_deviceInit(USART_HandleTypeDef *handle, uint32_t baudRate) {

	HAL_StatusTypeDef result = HAL_ERROR;
	USART_InitTypeDef ifaceParams = {
			baudRate,
			USART_WORDLENGTH_8B,
			USART_STOPBITS_1,
			USART_PARITY_EVEN,
			USART_MODE_TX_RX,
			USART_POLARITY_LOW,
			0,0
	};
	if (handle) {
		s_deviceHandle = s_deviceHandle;
		memset(handle, 0, sizeof(*handle));
		handle->Instance = USART2;
		handle->Init = ifaceParams;

		result = HAL_USART_Init(handle);
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
