/*
 * timers.h
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx.h"

#define INVALID_HANDLE (0)

typedef void (*onTimerFire_t)(void *);

uint32_t Timer_newArmed(uint32_t tout, _Bool isPeriodic, onTimerFire_t cb, void *cbData);
void Timer_delete(uint32_t id);
void Timer_rearm(uint32_t id);
void Timer_disarm(uint32_t id);
void Timer_makeTick(void);

#endif /* TIMERS_H_ */
