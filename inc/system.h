/*
 * systemStatus.h
 *
 *  Created on: Jan 9, 2016
 *      Author: pavelgamov
 */

#ifndef SYSTEM_H_
#define SYSTEM_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

typedef enum {
	INFORM_INIT,
	INFORM_PREHEAT,
	INFORM_IDLE,
	INFORM_SLEEP,
	INFORM_CONNECTION_LOST,
	INFORM_ERROR,
	INFORM_LAST
} systemStatus_t;

typedef void (*ledOutputControl_t)(FunctionalState);

void System_setLedControl(ledOutputControl_t);
void System_setStatus(systemStatus_t);

uint32_t System_getUptime(void);
uint32_t System_getUptimeMs(void);

void System_delayMsDummy(uint32_t delay);


#endif /* SYSTEM_H_ */
