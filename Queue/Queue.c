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

static EventQueue_p newNodeLocked(Event_p pEvent);

EventQueue_p Queue_pushEvent(EventQueue_p pQueue, Event_p pEvent) {
	do {
		if (!pEvent)
			break;
		__disable_irq();
		if (!pQueue) {
			pQueue = newNodeLocked(pEvent);
			break;
		}
		EventQueue_p newNode = newNodeLocked(pEvent);
		if (!newNode)
			break;
		if (pQueue->last) {
			pQueue->last->next = newNode;
			pQueue->last = newNode;
		} else {
			pQueue->last = newNode;
			pQueue->next = pQueue->last;
		}
	} while (0);
	__enable_irq();
	return pQueue;
}

EventQueue_p Queue_getEvent(EventQueue_p pQueue, Event_p pEvent) {
	if (!pQueue || !pEvent)
		return pQueue;
	__disable_irq();
	do {
		EventQueue_p cur = pQueue;
		*pEvent = pQueue->event;
		pQueue = pQueue->next;
		free(cur);
	} while (0);
	__enable_irq();
	return pQueue;
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
