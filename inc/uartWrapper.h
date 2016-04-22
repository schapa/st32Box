/*
 * usartWrapper.h
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#ifndef UARTWRAPPER_H_
#define UARTWRAPPER_H_

#include "stm32f4xx_hal.h"
#include "Queue.h"

HAL_StatusTypeDef UART4_Init(UART_HandleTypeDef *handle, uint32_t baudRate);

void UART_handleEvent(Event_p event);

#endif /* USARTWRAPPER_H_ */
