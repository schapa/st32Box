/*
 * misc.h
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#ifndef MISC_H_
#define MISC_H_

#include "stm32f4xx_hal.h"

void printHelpMessage(void);
void dumpUsartTraceProps(USART_HandleTypeDef *traceUsart);
void dumpCANProps(CAN_HandleTypeDef *canBus);

#endif /* MISC_H_ */
