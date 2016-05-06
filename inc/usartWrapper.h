/*
 * usartWrapper.h
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#ifndef USARTWRAPPER_H_
#define USARTWRAPPER_H_

#include "stm32f4xx_hal.h"
#include <stdbool.h>

HAL_StatusTypeDef Trace_InitUSART1(USART_HandleTypeDef *handle, DMA_HandleTypeDef *txDmaHandle);
_Bool Trace_onTxComplete(USART_HandleTypeDef *handle);
void Trace_dataAsync(char *buff, size_t size);

HAL_StatusTypeDef USART2_Init(USART_HandleTypeDef *handle, uint32_t baudRate);

#endif /* USARTWRAPPER_H_ */
