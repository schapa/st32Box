/*
 * bsp.h
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#ifndef BSP_H_
#define BSP_H_

#include "stm32f4xx_hal.h"

void LED_init(void);
void Led_Red_SetState(FunctionalState state);
void Led_Green_SetState(FunctionalState state);



#endif /* BSP_H_ */
