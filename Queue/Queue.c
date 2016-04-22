/*
 * Queue.c
 *
 *  Created on: Apr 20, 2016
 *      Author: shapa
 */

#include "Queue.h"
#include <stdlib.h>
#include <string.h>
#include "stm32f4xx_hal.h"

static Event_t s_dummyEvent;

static _Bool isSameEvent(Event_p pEventOne, Event_p pEventTwo);
static EventQueue_p newNodeLocked(Event_p pEvent);
static EventQueue_p findBeforeLocked(EventQueue_p pQueue, EventQueue_p pElt);

_Bool Queue_new(EventQueue_p *pQueue) {
	_Bool result = false;
	do {
		if (!pQueue )
			break;
		__disable_irq();
		*pQueue = newNodeLocked(&s_dummyEvent);
		result = !!(*pQueue);
		__enable_irq();
	} while (0);
	return result;
}

_Bool Queue_pushEvent(EventQueue_p pQueue, Event_p pEvent) {
	_Bool result = false;
	do {
		if (!pQueue || !pEvent)
			break;
		__disable_irq();
		if (!pQueue->last) {
			EventQueue_p newNode = newNodeLocked(pEvent);
			if (newNode) {
				pQueue->last = newNode;
				pQueue->next = pQueue->last;
				result = true;
			}
		} else {
			EventQueue_p newNode = newNodeLocked(pEvent);
			if (newNode) {
				pQueue->last->next = newNode;
				pQueue->last = pQueue->last->next;
				result = true;
			}
		}
		__enable_irq();

	} while (0);
	return result;
}

_Bool Queue_getEvent(EventQueue_p pQueue, Event_p pEvent, _Bool isBlockingMode) {
	_Bool result = false;
	if (!pQueue || !pEvent)
		return result;
	do {
		__disable_irq();
		EventQueue_p cur = isSameEvent(&pQueue->last->event, &s_dummyEvent) ? NULL : pQueue->last;
		if (!cur && isBlockingMode) {
			__enable_irq();
			__WFI();
		} else {
			*pEvent = cur->event;
			pQueue->last = findBeforeLocked(pQueue, cur);
			if (pQueue->last)
				pQueue->last->next = NULL;
			free(cur);
			result = true;
		}
	} while (!result && isBlockingMode);
	__enable_irq();
	return result;
}

static _Bool isSameEvent(Event_p pEventOne, Event_p pEventTwo) {
	return !memcmp(pEventOne, pEventTwo, sizeof(Event_t));
}

static EventQueue_p newNodeLocked(Event_p pEvent) {
	EventQueue_p node = malloc(sizeof(EventQueue_t));
	if (node) {
		node->event = *pEvent;
		node->next = NULL;
		node->last = NULL;
	}
	return node;
}

static EventQueue_p findBeforeLocked(EventQueue_p pQueue, EventQueue_p pElt) {
	if (!pQueue || !pElt || (pQueue == pElt))
		return NULL;
	while (pQueue && (pQueue->next != pElt)) {
		pQueue = pQueue->next;
	}
	return pQueue;
}
