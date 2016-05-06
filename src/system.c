/*
 * systemTimer.c
 *
 *  Created on: April 2, 2016
 *      Author: pavelgamov
 */

#include <stddef.h>
#include <system.h>

#include "bsp.h"
#include "qpc.h"

static struct {
	uint32_t activeTime;
	uint32_t passiveTime;
} s_timing[] = {
		[INFORM_INIT] = { 0.1*BSP_TICKS_PER_SECOND, 0.3*BSP_TICKS_PER_SECOND },
		[INFORM_PREHEAT] = { 1*BSP_TICKS_PER_SECOND, 0.25*BSP_TICKS_PER_SECOND },
		[INFORM_IDLE] = { 0.1*BSP_TICKS_PER_SECOND, BSP_TICKS_PER_SECOND },
		[INFORM_SLEEP] = { 0.05*BSP_TICKS_PER_SECOND, 2*BSP_TICKS_PER_SECOND},
		[INFORM_CONNECTION_LOST] = { 0.1*BSP_TICKS_PER_SECOND, 0.5*BSP_TICKS_PER_SECOND},
		[INFORM_ERROR] = { 0.05*BSP_TICKS_PER_SECOND, 0.05*BSP_TICKS_PER_SECOND},
};

static systemStatus_t s_systemStatus = INFORM_ERROR;
static uint32_t s_systemStatusTimer = 0;
static ledOutputControl_t s_systemLed = NULL;
static volatile uint32_t s_delayDecrement = 0;
static volatile uint32_t s_uptimeSeconds = 0;
static volatile uint32_t s_uptimeTicks = 1;

void System_setLedControl(ledOutputControl_t control) {
	s_systemLed = control;
}

void System_setStatus(systemStatus_t status) {
	if(status < INFORM_LAST) {
		s_systemStatus = status;
	}
}

void SysTick_Handler(void) {
	uint32_t period = s_timing[s_systemStatus].activeTime + s_timing[s_systemStatus].passiveTime;
	if (s_systemLed)
		s_systemLed(s_systemStatusTimer <= s_timing[s_systemStatus].activeTime);
	if (++s_systemStatusTimer > period) {
		s_systemStatusTimer = 0;
	}
	if (s_delayDecrement)
		s_delayDecrement--;

	if (!(s_uptimeTicks++ % BSP_TICKS_PER_SECOND)) {
		s_uptimeSeconds++;
		Event_t seconds = { EVENT_SYSTICK, { ES_SYSTICK_SECOND_ELAPSED }, .data.intptr = s_uptimeSeconds };
		BSP_queuePush(&seconds);
	}
#if defined(USE_HAL_DRIVER)
	HAL_IncTick();
#endif
	QF_TICK_X(0U, (void *)0);
}

void System_delayMsDummy(uint32_t delay) {
	s_delayDecrement = delay;
	while (s_delayDecrement);
}

uint32_t System_getUptime(void) {
	return s_uptimeSeconds;
}
uint32_t System_getUptimeMs(void) {
	return s_uptimeTicks%BSP_TICKS_PER_SECOND;
}
