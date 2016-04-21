/*
 * Queue.h
 *
 *  Created on: Apr 20, 2016
 *      Author: shapa
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdint.h>
#include <stdbool.h>
#include "Events.h"

typedef struct Event Event_t, *Event_p;
typedef struct Queue EventQueue_t, *EventQueue_p;

struct Event {
	EventTypes_t type;
	EventSubTypes_t subType;
	EventDataTypes_t data;
};

struct Queue {
	Event_t event;
	EventQueue_p next;
	EventQueue_p last;
};

_Bool Queue_new(EventQueue_p *pQueue);
_Bool Queue_pushEvent(EventQueue_p pQueue, Event_p pEvent);
_Bool Queue_getEvent(EventQueue_p pQueue, Event_p pEvent, _Bool isBlockingMode);


#endif /* QUEUE_H_ */
