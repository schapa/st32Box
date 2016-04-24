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

#define INVALID_HANDLE (0)

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

typedef void (*onTimerFire_t)(void *);

void SystemStatus_setLedControl(ledOutputControl_t);
void SystemStatus_set(systemStatus_t);

void SystemTimer_delayMsDummy(uint32_t delay);

uint32_t SystemStatus_getUptime(void);

uint32_t SystemTimer_newArmed(uint32_t tout, _Bool isPeriodic, onTimerFire_t cb, void *cbData);
void SystemTimer_rearm(uint32_t id);
void SystemTimer_disarm(uint32_t id);


#endif /* SYSTEMSTATUS_H_ */
