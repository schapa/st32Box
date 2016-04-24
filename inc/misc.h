/*
 * misc.h
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#ifndef MISC_H_
#define MISC_H_

#include "stm32f4xx_hal.h"
#include "diag/Trace.h"

void HELP_printMessage(void);
void HELP_dumpUsartProps(USART_HandleTypeDef *usart);
void HELP_dumpUartProps(UART_HandleTypeDef *uart);
void HELP_dumpCANProps(CAN_HandleTypeDef *canBus);
void HELP_dumpCANRxMsg(CanRxMsgTypeDef *msg);

uint32_t HELP_getUsartIdByHandle(USART_HandleTypeDef *usart);
uint32_t HELP_getUartIdByHandle(UART_HandleTypeDef *uart) ;
uint32_t HELP_getCanIdByHandle(CAN_HandleTypeDef *canBus);

#endif /* MISC_H_ */
