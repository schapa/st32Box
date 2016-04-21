/*
 * help.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */


#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "misc.h"

static void dumpTargetProps(void);
static uint32_t usartIdByHandle(USART_TypeDef *usart);
static const char *usartStrGPIO(USART_TypeDef *usart);
static const char *usartStrWordLength(uint32_t wordLength);
static const char *usartStrStopBits(uint32_t stopBits);
static const char *usartStrParity(uint32_t parity);
static const char *usartStrMode(uint32_t mode);

static const char *canStrGPIO(CAN_TypeDef *handle);
static uint32_t canIdByHandle(CAN_TypeDef *can);

static const char *const delim = "----------------------------------------------------";


void HELP_printMessage(void) {
	trace_printf("\n\r%s\n\r", delim);
	dumpTargetProps();
	trace_printf("%s\n\r", delim);
}


void HELP_dumpUsartProps(USART_HandleTypeDef *traceUsart) {
	if (traceUsart) {
		trace_printf("USART_%d\n\r", usartIdByHandle(traceUsart->Instance));
		trace_printf("\t        Pins: %s\n\r", usartStrGPIO(traceUsart->Instance));
		trace_printf("\t   Baud rate: %d\n\r", traceUsart->Init.BaudRate);
		trace_printf("\t Word length: %s\n\r", usartStrWordLength(traceUsart->Init.WordLength));
		trace_printf("\t      Parity: %s\n\r", usartStrParity(traceUsart->Init.Parity));
		trace_printf("\t   Stop bits: %s\n\r", usartStrStopBits(traceUsart->Init.StopBits));
		trace_printf("\t        Mode: %s\n\r", usartStrMode(traceUsart->Init.Mode));
	}
}

void HELP_dumpCANProps(CAN_HandleTypeDef *canBus) {
	if (canBus) {
		trace_printf("CAN_%d\n\r", canIdByHandle(canBus->Instance));
		trace_printf("\t        Pins: %s\n\r", canStrGPIO(canBus->Instance));
	}
}

void HELP_dumpCANRxMsg(CanRxMsgTypeDef *msg) {
	uint32_t id = msg->IDE ? msg->ExtId : msg->StdId;
	if (msg->RTR) {
		trace_printf("RTR [%p] \n\r", id);
	} else {
		trace_printf("DLC [%p] = [", id);
		for (uint32_t i = 0; i < msg->DLC; i++)
			trace_printf(" %d", msg->Data[i]);
		trace_printf("] = %d\n\r", msg->DLC);
	}
}

uint32_t HELP_getUsartIdByHandle(USART_HandleTypeDef *traceUsart) {
	return traceUsart ? usartIdByHandle(traceUsart->Instance) : 0;
}

uint32_t HELP_getCanIdByHandle(CAN_HandleTypeDef *canBus) {
	return canBus ? canIdByHandle(canBus->Instance) : 0;
}


static void dumpTargetProps(void) {
	trace_printf("Target:\n\r");
	trace_printf("\t DEV_ID: %p\n\r", HAL_GetDEVID());
	trace_printf("\t REV_ID: %p\n\r", HAL_GetREVID());
	trace_printf("\t SYSCLK: %d\n\r", HAL_RCC_GetSysClockFreq());
	trace_printf("\t   HCLK: %d\n\r", HAL_RCC_GetHCLKFreq());
	trace_printf("\t  PCLK1: %d\n\r", HAL_RCC_GetPCLK1Freq());
	trace_printf("\t  PCLK2: %d\n\r", HAL_RCC_GetPCLK2Freq());
	trace_printf("HalVersion: %p\n\r", HAL_GetHalVersion());
}

static uint32_t usartIdByHandle(USART_TypeDef *usart) {
	static const struct {
		USART_TypeDef *handle;
		uint32_t id;
	} elements[] = {
			{ USART1, 1 },
			{ USART2, 2 },
			{ USART3, 3 },
			{ USART6, 6 },
	};
	static const size_t elementsCount = sizeof(elements)/sizeof(*elements);
	size_t i;
	uint32_t id = 0;
	for (i = 0; i < elementsCount; i++) {
		if (usart == elements[i].handle) {
			id = elements[i].id;
			break;
		}
	}
	return id;
}

static uint32_t canIdByHandle(CAN_TypeDef *can) {
	static const struct {
		CAN_TypeDef *handle;
		uint32_t id;
	} elements[] = {
			{ CAN1, 1 },
			{ CAN2, 2 },
	};
	static const size_t elementsCount = sizeof(elements)/sizeof(*elements);
	size_t i;
	uint32_t id = 0;
	for (i = 0; i < elementsCount; i++) {
		if (can == elements[i].handle) {
			id = elements[i].id;
			break;
		}
	}
	return id;
}

static const char *usartStrGPIO(USART_TypeDef *usart) {
	static const struct {
		USART_TypeDef *handle;
		const char *str;
	} elements[] = {
			{ USART1, "TX:PA9 RX:PA10" },
			{ USART2, "TX:PD5 RX:PD6" },
			{ USART3, "(Unknown)" },
			{ USART6, "(Unknown)" },
	};
	static const size_t elementsCount = sizeof(elements)/sizeof(*elements);
	size_t i;
	const char *str = 0;
	for (i = 0; i < elementsCount; i++) {
		if (usart == elements[i].handle) {
			str = elements[i].str;
			break;
		}
	}
	return str;
}

static const char *usartStrWordLength(uint32_t wordLength) {
	if (wordLength == USART_WORDLENGTH_8B)
		return "8B";
	if (wordLength == USART_WORDLENGTH_9B)
		return "9B";
	return NULL;
}

static const char *usartStrStopBits(uint32_t stopBits) {
	if (stopBits == USART_STOPBITS_1)
		return "1";
	if (stopBits == USART_STOPBITS_0_5)
		return "0.5";
	if (stopBits == USART_STOPBITS_2)
		return "2";
	if (stopBits == USART_STOPBITS_1_5)
		return "1.5";
	return NULL;
}

static const char *usartStrParity(uint32_t parity) {
	if (parity == USART_PARITY_NONE)
		return "NONE";
	if (parity == USART_PARITY_EVEN)
		return "EVEN";
	if (parity == USART_PARITY_ODD)
		return "ODD";
	return NULL;
}

static const char *usartStrMode(uint32_t mode) {
	if (mode == USART_MODE_RX)
		return "RX";
	if (mode == USART_MODE_TX)
		return "TX";
	if (mode == USART_MODE_TX_RX)
		return "RX_TX";
	return NULL;
}

static const char *canStrGPIO(CAN_TypeDef *handle) {
	static const struct {
		CAN_TypeDef *handle;
		const char *str;
	} elements[] = {
			{ CAN1, "TX:PA12 RX:PA11" },
			{ CAN2, "(Unknown)" },
	};
	static const size_t elementsCount = sizeof(elements)/sizeof(*elements);
	size_t i;
	const char *str = NULL;
	for (i = 0; i < elementsCount; i++) {
		if (handle == elements[i].handle) {
			str = elements[i].str;
			break;
		}
	}
	return str;
}

