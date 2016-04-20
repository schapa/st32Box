/*
 * misc.h
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#ifndef MISC_H_
#define MISC_H_

#include "stm32f4xx_hal.h"

void HELP_printMessage(void);
void HELP_dumpUsartProps(USART_HandleTypeDef *traceUsart);
void HELP_dumpCANProps(CAN_HandleTypeDef *canBus);

uint32_t HELP_getUsartIdByHandle(USART_HandleTypeDef *traceUsart);

#endif /* MISC_H_ */
