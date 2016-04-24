/*
 * systemTimer.c
 *
 *  Created on: April 2, 2016
 *      Author: pavelgamov
 */

#include <stddef.h>

#include "bsp.h"
#include "systemStatus.h"
#include "memman.h"

#define TICKS_PER_SECOND 1000

typedef struct timerNode {
	struct timerNode *next;
	uint32_t id;
	uint32_t timeout;
	 _Bool isPeriodic;
	 onTimerFire_t cb;
	 void *cbData;
	 uint32_t cnt;
} timerNode_t, *timerNode_p;

static struct {
	uint32_t activeTime;
	uint32_t passiveTime;
} s_timing[] = {
		[INFORM_INIT] = { 0.1*TICKS_PER_SECOND, 0.3*TICKS_PER_SECOND },
		[INFORM_PREHEAT] = { 1*TICKS_PER_SECOND, 0.25*TICKS_PER_SECOND },
		[INFORM_IDLE] = { 0.1*TICKS_PER_SECOND, TICKS_PER_SECOND },
		[INFORM_SLEEP] = { 0.05*TICKS_PER_SECOND, 2*TICKS_PER_SECOND},
		[INFORM_CONNECTION_LOST] = { 0.1*TICKS_PER_SECOND, 0.5*TICKS_PER_SECOND},
		[INFORM_ERROR] = { 0.05*TICKS_PER_SECOND, 0.05*TICKS_PER_SECOND},
};

static systemStatus_t s_systemStatus = INFORM_ERROR;
static uint32_t s_systemStatusTimer = 0;
static ledOutputControl_t s_systemLed = NULL;
static volatile uint32_t s_delayDecrement = 0;
static volatile uint32_t s_uptimeSeconds = 0;
static volatile uint32_t s_uptimeTicks = 0;
static timerNode_p s_timersListHead = NULL;
static timerNode_p s_timersListTail = NULL;

static uint32_t getNewHandle(void);
static _Bool isTimerHandleUnique(uint32_t handle);
static timerNode_p findTimerById(uint32_t handle);
static void tickTimers(void);

void SystemStatus_setLedControl(ledOutputControl_t control) {
	s_systemLed = control;
}

void SystemStatus_set(systemStatus_t status) {
	if(status < INFORM_LAST) {
		s_systemStatus = status;
	}
}

void SysTick_Handler(void) {
	uint32_t period = s_timing[s_systemStatus].activeTime + s_timing[s_systemStatus].passiveTime;
//	Event_t tick = { EVENT_SYSTICK, { ES_SYSTICK_TICK }, .data.intptr = s_uptimeTicks };
//	BSP_queuePush(&tick);
	if (s_systemLed)
		s_systemLed(s_systemStatusTimer <= s_timing[s_systemStatus].activeTime);
	if (++s_systemStatusTimer > period) {
		s_systemStatusTimer = 0;
	}
	if (s_delayDecrement)
		s_delayDecrement--;

	if (!(s_uptimeTicks++ % TICKS_PER_SECOND)) {
		s_uptimeSeconds++;
		Event_t seconds = { EVENT_SYSTICK, { ES_SYSTICK_SECOND_ELAPSED }, .data.intptr = s_uptimeSeconds };
		BSP_queuePush(&seconds);
	}
	tickTimers();
#if defined(USE_HAL_DRIVER)
	HAL_IncTick();
#endif
}

void SystemTimer_delayMsDummy(uint32_t delay) {
	s_delayDecrement = delay;
	while (s_delayDecrement);
}

uint32_t SystemStatus_getUptime(void) {
	return s_uptimeSeconds;
}

uint32_t SystemTimer_newArmed(uint32_t tout, _Bool isPeriodic, onTimerFire_t cb, void *cbData) {
	uint32_t handle = INVALID_HANDLE;
	if (!cb || !tout)
		return INVALID_HANDLE;
	do {
		handle = getNewHandle();
	} while (!isTimerHandleUnique(handle));
	timerNode_p last = s_timersListTail;
	if (!last && (last = MEMMAN_malloc(sizeof(timerNode_t)))) {
		last->id = handle;
		last->cnt = last->timeout = tout;
		last->isPeriodic = isPeriodic;
		last->cb = cb;
		last->cbData = cbData;
		last->next = NULL;
		s_timersListTail = s_timersListHead = last;
	}

	return handle;
}

void SystemTimer_rearm(uint32_t id) {
	if (id == INVALID_HANDLE)
		return;
	timerNode_p node = findTimerById(id);
	if (node) {
		node->cnt = node->timeout;
	}
}

void SystemTimer_disarm(uint32_t id) {
	if (id == INVALID_HANDLE)
		return;
	timerNode_p node = s_timersListHead;
	timerNode_p prev = NULL;
	while (node) {
		if (node->id == id)
			break;
		prev = node;
		node = node->next;
	}
	if (prev) {
		prev = node->next;
		if (node == s_timersListTail)
			s_timersListTail = prev;
		MEMMAN_free(node);
	} else {
		MEMMAN_free(s_timersListHead);
		s_timersListHead = s_timersListTail = NULL;
	}
}

static uint32_t getNewHandle(void) {
	static uint32_t handle = 0;
	if (!handle || !++handle)
		handle = 1;
	return handle;
}

static _Bool isTimerHandleUnique(uint32_t handle) {
	timerNode_p node = s_timersListHead;
	while (node) {
		if (node->id = handle)
			return false;
		node = node->next;
	}
	return true;
}

static timerNode_p findTimerById(uint32_t handle) {
	timerNode_p node = s_timersListHead;
	while (node) {
		if (handle == node->id)
			return node;
		node = node->next;
	}
	return NULL;
}

static void tickTimers(void) {
	timerNode_p node = s_timersListHead;
	while (node) {
		if (!node->cnt--) {
			node->cb(node->cbData);
			if (node->isPeriodic)
				node->cnt = node->timeout;
			else {
				uint32_t id = node->id;
				node = node->next;
				SystemTimer_disarm(id);
				continue;
			}
		}
		node = node->next;
	}
}
