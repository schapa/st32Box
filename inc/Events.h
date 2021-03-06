/*
 * Events.h
 *
 *  Created on: Apr 20, 2016
 *      Author: shapa
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include "stm32f4xx_hal.h"

typedef enum {
	EVENT_DUMMY,
	EVENT_SYSTICK,
	EVENT_EXTI,
	EVENT_UART,
	EVENT_USART,
	EVENT_UxART_Buffer,
	EVENT_CAN,
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
	ES_UxART_RX,
	ES_UxART_TX,
	ES_UxART_RXTX,
	ES_UxART_ERROR
} UxartSubTypes_t;
typedef struct {
	union {
		USART_HandleTypeDef *hUsart;
		UART_HandleTypeDef *hUart;
	};
	intptr_t buffer;
	size_t size;
} UxartDataTypes_t;

typedef enum {
	ES_CAN_RX,
	ES_CAN_TX,
	ES_CAN_ERROR
} CanSubTypes_t;
typedef struct {
	CAN_HandleTypeDef *hCan;
	union {
		CanTxMsgTypeDef *txMsg;
		CanRxMsgTypeDef *rxMsg;
	};
} CanDataTypes_t;

typedef union {
	SystickSubTypes_t systick;
	ExtiSubTypes_t exti;
	UxartSubTypes_t uxart;
	CanSubTypes_t can;
} EventSubTypes_t;

typedef union {
	intptr_t intptr;
	UxartDataTypes_t uxart;
	CanDataTypes_t can;
} EventDataTypes_t;


typedef struct {
	EventTypes_t type;
	EventSubTypes_t subType;
	EventDataTypes_t data;
} Event_t, *Event_p;

#endif /* EVENTS_H_ */
