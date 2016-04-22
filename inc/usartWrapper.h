/*
 * usartWrapper.h
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#ifndef USARTWRAPPER_H_
#define USARTWRAPPER_H_

#include "stm32f4xx_hal.h"

HAL_StatusTypeDef USART1_InitTrace(USART_HandleTypeDef *handle);
HAL_StatusTypeDef USART2_Init(USART_HandleTypeDef *handle, uint32_t baudRate);

#endif /* USARTWRAPPER_H_ */
