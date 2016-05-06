/*
 * systemStatus.h
 *
 *  Created on: Jan 9, 2016
 *      Author: pavelgamov
 */

#ifndef SYSTEMSTATUS_H_
#define SYSTEMSTATUS_H_

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

void SystemStatus_setLedControl(ledOutputControl_t);
void SystemStatus_set(systemStatus_t);

uint32_t System_getUptime(void);
uint32_t System_getUptimeMs(void);

void SystemTimer_delayMsDummy(uint32_t delay);


#endif /* SYSTEMSTATUS_H_ */
