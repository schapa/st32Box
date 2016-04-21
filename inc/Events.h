/*
 * Events.h
 *
 *  Created on: Apr 20, 2016
 *      Author: shapa
 */

#ifndef EVENTS_H_
#define EVENTS_H_

typedef enum {
	EVENT_DUMMY,
	EVENT_SYSTICK,
	EVENT_EXTI,
	EVENT_UART,
	EVENT_USART,
	EVENT_LAST
} EventTypes_t;

typedef enum {
	ES_SYSTICK_TICK,
	ES_SYSTICK_SECOND_ELAPSED
} SystickSubTypes_t;

typedef enum {
	ES_EXTI_DOWN,
	ES_EXTI_UP,
} ExtiSubTypes_t;

typedef enum {
	ES_UsART_RX,
	ES_UsART_TX,
	ES_UsART_RXTX,
	ES_UsART_ERROR
} UsartSubTypes_t;

typedef union {
	SystickSubTypes_t systick;
	ExtiSubTypes_t exti;
	UsartSubTypes_t uxart;
} EventSubTypes_t;




#endif /* EVENTS_H_ */
