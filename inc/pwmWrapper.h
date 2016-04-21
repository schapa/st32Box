/*
 * pwmWrapper.h
 *
 *  Created on: Apr 8, 2016
 *      Author: pavelgamov
 */

#ifndef PWMWRAPPER_H_
#define PWMWRAPPER_H_

#include <stdint.h>

void PWM_Init(void);
void PWM_Configure(uint32_t channel, uint8_t dutPercent);

#endif /* PWMWRAPPER_H_ */
