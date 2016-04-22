/*
 * uartWrapper.c
 *
 *  Created on: Apr 22, 2016
 *      Author: shapa
 */

#include "uartWrapper.h"
#include "string.h"
#include "bsp.h"

static UART_HandleTypeDef *s_deviceHandle = NULL;

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
		s_deviceHandle = handle;
		memset(handle, 0, sizeof(*handle));
		handle->Instance = UART4;
		handle->Init = ifaceParams;

		result = HAL_UART_Init(handle);
		HAL_NVIC_EnableIRQ(UART4_IRQn);
	}
	return result;
}

void UART4_IRQHandler(void) {
	HAL_UART_IRQHandler(s_deviceHandle);
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
	Event_t event = { EVENT_UART, { ES_UsART_RX },
			.data.uart = { huart }
	};
	BSP_queuePush(&event);
}
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
	Event_t event = { EVENT_UART, { ES_UsART_TX },
			.data.uart = { huart }
	};
	BSP_queuePush(&event);
}
void HAL_UART_TxRxCpltCallback(UART_HandleTypeDef *huart) {
	Event_t event = { EVENT_UART, { ES_UsART_RXTX },
			.data.uart = { huart }
	};
	BSP_queuePush(&event);
}
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
	Event_t event = { EVENT_UART, { ES_UsART_ERROR },
			.data.uart = { huart }
	};
	BSP_queuePush(&event);
}
