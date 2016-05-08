/*
 * bsp.h
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#ifndef BSP_H_
#define BSP_H_

#include "stm32f4xx_hal.h"
#include "Events.h"
#include "config.h"

#define BSP_TICKS_PER_SECOND 1000

void BSP_init(void);

void BSP_queuePush(Event_p pEvent);

void BSP_espSend(const char *buff, size_t size);

void Led_Red_SetState(FunctionalState state);
void Led_Green_SetState(FunctionalState state);

_Bool Char_isTerminal(char symb);

#endif /* BSP_H_ */
