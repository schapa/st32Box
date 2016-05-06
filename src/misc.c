/*
 * help.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#include "stm32f4xx_hal.h"
#include "misc.h"
#include "dbg_base.h"

#if 0
#include "dbg_trace.h"
#endif

static void dumpTargetProps(void);
static uint32_t uxartIdByHandle(USART_TypeDef *usart);
static const char *uxartStrGPIO(USART_TypeDef *usart);
static const char *usartStrWordLength(uint32_t wordLength);
static const char *uartStrWordLength(uint32_t wordLength);
static const char *usartStrStopBits(uint32_t stopBits);
static const char *uartStrStopBits(uint32_t stopBits);
static const char *usartStrParity(uint32_t parity);
static const char *uartStrParity(uint32_t parity);
static const char *usartStrMode(uint32_t mode);
static const char *uartStrMode(uint32_t mode);

static const char *canStrGPIO(CAN_TypeDef *handle);
static uint32_t canIdByHandle(CAN_TypeDef *can);

static const char *const delim = "----------------------------------------------------";


void HELP_printMessage(void) {
	DBGMSG_L("%s", delim);
	dumpTargetProps();
	DBGMSG_H("%s", delim);
}


void HELP_dumpUsartProps(USART_HandleTypeDef *traceUsart) {
	if (traceUsart) {
		DBGMSG_H("USART_%d", uxartIdByHandle(traceUsart->Instance));
		DBGMSG_L("        Pins: %s", uxartStrGPIO(traceUsart->Instance));
		DBGMSG_L("   Baud rate: %d", traceUsart->Init.BaudRate);
		DBGMSG_L(" Word length: %s", usartStrWordLength(traceUsart->Init.WordLength));
		DBGMSG_L("      Parity: %s", usartStrParity(traceUsart->Init.Parity));
		DBGMSG_L("   Stop bits: %s", usartStrStopBits(traceUsart->Init.StopBits));
		DBGMSG_L("        Mode: %s", usartStrMode(traceUsart->Init.Mode));
	}
}

void HELP_dumpUartProps(UART_HandleTypeDef *uart) {
	if (uart) {
		DBGMSG_H("UART_%d", uxartIdByHandle(uart->Instance));
		DBGMSG_L("        Pins: %s", uxartStrGPIO(uart->Instance));
		DBGMSG_L("   Baud rate: %d", uart->Init.BaudRate);
		DBGMSG_L(" Word length: %s", uartStrWordLength(uart->Init.WordLength));
		DBGMSG_L("      Parity: %s", uartStrParity(uart->Init.Parity));
		DBGMSG_L("   Stop bits: %s", uartStrStopBits(uart->Init.StopBits));
		DBGMSG_L("        Mode: %s", uartStrMode(uart->Init.Mode));
	}
}

void HELP_dumpCANProps(CAN_HandleTypeDef *canBus) {
	if (canBus) {
		DBGMSG_H("CAN_%d", canIdByHandle(canBus->Instance));
		DBGMSG_L("        Pins: %s", canStrGPIO(canBus->Instance));
	}
}

void HELP_dumpCANRxMsg(CanRxMsgTypeDef *msg) {
	uint32_t id = msg->IDE ? msg->ExtId : msg->StdId;
	if (msg->RTR) {
		DBGMSG_L("RTR [%p] ", id);
	} else {
		DBGMSG_L("DLC [%p] = [", id);
		for (uint32_t i = 0; i < msg->DLC; i++)
			DBGMSG_L(" %d", msg->Data[i]);
		DBGMSG_L("] = %d", msg->DLC);
	}
}

uint32_t HELP_getUsartIdByHandle(USART_HandleTypeDef *usart) {
	return usart ? uxartIdByHandle(usart->Instance) : 0;
}

uint32_t HELP_getUartIdByHandle(UART_HandleTypeDef *uart) {
	return uart ? uxartIdByHandle(uart->Instance) : 0;
}

uint32_t HELP_getCanIdByHandle(CAN_HandleTypeDef *canBus) {
	return canBus ? canIdByHandle(canBus->Instance) : 0;
}


static void dumpTargetProps(void) {
	DBGMSG_H("Target:");
	DBGMSG_L(" DEV_ID: %p", HAL_GetDEVID());
	DBGMSG_L(" REV_ID: %p", HAL_GetREVID());
	DBGMSG_L(" SYSCLK: %d", HAL_RCC_GetSysClockFreq());
	DBGMSG_L("   HCLK: %d", HAL_RCC_GetHCLKFreq());
	DBGMSG_L("  PCLK1: %d", HAL_RCC_GetPCLK1Freq());
	DBGMSG_L("  PCLK2: %d", HAL_RCC_GetPCLK2Freq());
	DBGMSG_H("HalVersion: %p", HAL_GetHalVersion());
}

static uint32_t uxartIdByHandle(USART_TypeDef *usart) {
	static const struct {
		USART_TypeDef *handle;
		uint32_t id;
	} elements[] = {
			{ USART1, 1 },
			{ USART2, 2 },
			{ USART3, 3 },
			{ UART4, 4 },
			{ UART5, 5 },
			{ USART6, 6 },
			{ UART7, 7 },
			{ UART8, 8 },
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

static const char *uxartStrGPIO(USART_TypeDef *usart) {
	static const struct {
		USART_TypeDef *handle;
		const char *str;
	} elements[] = {
			{ USART1, "TX:PA9 RX:PA10" },
			{ USART2, "TX:PD5 RX:PD6" },
			{ USART3, "(Unknown)" },
			{ UART4,  "TX:PC10 RX:PC11" },
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

static const char *uartStrWordLength(uint32_t wordLength) {
	if (wordLength == UART_WORDLENGTH_8B)
		return "8B";
	if (wordLength == UART_WORDLENGTH_9B)
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

static const char *uartStrStopBits(uint32_t stopBits) {
	if (stopBits == UART_STOPBITS_1)
		return "1";
	if (stopBits == UART_STOPBITS_2)
		return "2";
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
static const char *uartStrParity(uint32_t parity) {
	if (parity == UART_PARITY_NONE)
		return "NONE";
	if (parity == UART_PARITY_EVEN)
		return "EVEN";
	if (parity == UART_PARITY_ODD)
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

static const char *uartStrMode(uint32_t mode) {
	if (mode == UART_MODE_RX)
		return "RX";
	if (mode == UART_MODE_TX)
		return "TX";
	if (mode == UART_MODE_TX_RX)
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

