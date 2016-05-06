/*
 * timers.c
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#include "timers.h"
#include "memman.h"

typedef struct timerNode {
	struct timerNode *next;
	uint32_t id;
	uint32_t timeout;
	 _Bool isPeriodic;
	 _Bool isActive;
	 onTimerFire_t cb;
	 void *cbData;
	 uint32_t cnt;
} timerNode_t, *timerNode_p;

static timerNode_p s_timersListHead = NULL;
static timerNode_p s_timersListTail = NULL;

static uint32_t getNewHandle(void);
static _Bool isTimerHandleUnique(uint32_t handle);
static timerNode_p findTimerById(uint32_t handle);

uint32_t Timer_newArmed(uint32_t tout, _Bool isPeriodic, onTimerFire_t cb, void *cbData) {
	uint32_t handle = INVALID_HANDLE;
	if (!cb || !tout)
		return INVALID_HANDLE;
	do {
		handle = getNewHandle();
	} while (!isTimerHandleUnique(handle));
	timerNode_p last = s_timersListTail;
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	if (!last && (last = MEMMAN_malloc(sizeof(timerNode_t)))) {
		last->id = handle;
		last->cnt = last->timeout = tout;
		last->isPeriodic = isPeriodic;
		last->isActive = true;
		last->cb = cb;
		last->cbData = cbData;
		last->next = NULL;
		s_timersListTail = s_timersListHead = last;
	}
	if (!primask) {
		__enable_irq();
	}

	return handle;
}

void Timer_rearm(uint32_t id) {
	if (id == INVALID_HANDLE)
		return;
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	timerNode_p node = findTimerById(id);
	if (node) {
		node->isActive = true;
		node->cnt = node->timeout;
	}
	if (!primask) {
		__enable_irq();
	}
}

void Timer_disarm(uint32_t id) {
	if (id == INVALID_HANDLE)
		return;
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	timerNode_p node = s_timersListHead;
	while (node) {
		if (node->id == id) {
			node->isActive = false;
			break;
		}
		node = node->next;
	}
	if (!primask) {
		__enable_irq();
	}
}

void Timer_delete(uint32_t id) {
	if (id == INVALID_HANDLE)
		return;
	timerNode_p node = s_timersListHead;
	timerNode_p prev = NULL;
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
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
	if (!primask) {
		__enable_irq();
	}
}

void Timer_makeTick(void) {
	timerNode_p node = s_timersListHead;
	uint32_t primask = __get_PRIMASK();
	__disable_irq();
	while (node) {
		if (node->isActive && !node->cnt--) {
			node->cb(node->cbData);
			if (node->isPeriodic)
				node->cnt = node->timeout;
			else {
				uint32_t id = node->id;
				node = node->next;
				Timer_delete(id);
				continue;
			}
		}
		node = node->next;
	}
	if (!primask) {
		__enable_irq();
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
		if (node->id == handle)
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
