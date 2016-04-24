/*
 * QueryTest.c
 *
 *  Created on: Apr 19, 2016
 *      Author: shapa
 */

#include "Query.h"
#include "stm32f4xx_hal.h"

#include "bsp.h"
#include "misc.h"
#include "uartWrapper.h"
#include "memman.h"
#include <string.h>

#define CRLF "\r\n"

typedef struct {
	const char *ssid;
	const char *passwd;
} DeviceConfig_t, *DeviceConfig_p;

static DeviceConfig_t config = {"shapa", "jnghfdkzq"};
static DeviceConfig_p s_config = &config;

static _Bool startRestartModule (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+RST"CRLF);
	return true;
}

static _Bool startEchoOff (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "ATE0"CRLF);
	return true;
}

static _Bool startModeChange (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CWMODE=1"CRLF);
	return true;
}

static _Bool listAccessPoints (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CWLAP"CRLF);
	return true;
}
static _Bool listAccessPointsOk (Request_p req) {
	trace_printf("[APS]\n\r%s\n\r", req->rx.buff);
	return true;
}

static _Bool startConnect (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CWJAP=\"%s\",\"%s\""CRLF, s_config->ssid, s_config->passwd);
	return true;
}

static _Bool getMyAdress (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIFSR"CRLF);
	return true;
}
static _Bool getMyAdressOk (Request_p req) {
	trace_printf("[IP]\n\r%s\n\r", req->rx.buff);
	return true;
}


static _Bool setMultipleMode (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPMUX=1"CRLF);
	return true;
}


static Step_t steps[] = {
		{ startRestartModule, NULL, NULL, STEP_INIT, 0, "ready" },
		{ startEchoOff, NULL, NULL, STEP_INIT, 0, NULL },
		{ startModeChange, NULL, NULL, STEP_INIT, 0, NULL },
		{ listAccessPoints, listAccessPointsOk, NULL, STEP_INIT, 0, NULL },
		{ startConnect, NULL, NULL, STEP_INIT, 0, NULL },
		{ getMyAdress, getMyAdressOk, NULL, STEP_INIT, 0, NULL },
		{ setMultipleMode, NULL, NULL, STEP_INIT, 0, NULL },
};

static char queryBuffer[1024];
static UART_HandleTypeDef s_uart;

static Request_t testReq = {
		0, QUERY_INIT,
		steps, sizeof(steps)/sizeof(*steps), 0,
		{ queryBuffer, sizeof(queryBuffer), 0 }
};


void QueryTest(uint8_t *buff, size_t size) {

	static _Bool init;
	if (!init) {
		UART4_Init(&s_uart, 115200);
		HELP_dumpUartProps(&s_uart);
		init = true;
	}
	if ((testReq.state != QUERY_FAILED) && (testReq.state != QUERY_DONE)) {
		if (buff && size) {
			char *start = MEMMAN_malloc(testReq.rx.occupied + size + 3);
			memset((void*)start, 0, testReq.rx.occupied + size + 3);
			char *ptr = start;
			if (testReq.rx.occupied) {
				memcpy((void*)start, (void*)testReq.rx.buff, testReq.rx.occupied);
				MEMMAN_free((void*)testReq.rx.buff);
				start[testReq.rx.occupied++] = '\n';
				start[testReq.rx.occupied++] = '\r';
				ptr = &start[testReq.rx.occupied];
			}
			memcpy((void*)ptr, (void*)buff, size);
			testReq.rx.buff = start;
			testReq.rx.occupied += size;
			start[testReq.rx.occupied] = '\0';
//			trace_printf("[QRX]\n\r%s\n\r", testReq.rx.buff);
		}
		QueryProcess(&testReq);
		HAL_UART_Transmit_IT(&s_uart, (uint8_t*)testReq.tx.buff, testReq.tx.occupied);
	}
}
