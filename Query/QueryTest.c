/*
 * QueryTest.c
 *
 *  Created on: Apr 19, 2016
 *      Author: shapa
 */

#include "Query.h"
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "usartWrapper.h"

#define CRLF "\r\n"

typedef struct {
	const char *ssid;
	const char *passwd;
} DeviceConfig_t, *DeviceConfig_p;

static DeviceConfig_t config = {"shapa", "jnghfdkzq"};
static DeviceConfig_p s_config = &config;

static _Bool startRestartModule (Request_p req) {
	char *buff = (char*)req->buffer;
	req->bufferOccupied = snprintf(buff, req->bufferSize, "AT+RST"CRLF);
	return true;
}

static _Bool startEchoOff (Request_p req) {
	char *buff = (char*)req->buffer;
	req->bufferOccupied = snprintf(buff, req->bufferSize, "ATE0"CRLF);
	return true;
}

static _Bool startModeChange (Request_p req) {
	char *buff = (char*)req->buffer;
	req->bufferOccupied = snprintf(buff, req->bufferSize, "AT+CWMODE=1"CRLF);
	return true;
}

static _Bool startConnect (Request_p req) {
	char *buff = (char*)req->buffer;
	req->bufferOccupied = snprintf(buff, req->bufferSize, "AT+CWJAP=\"%s\",\"%s\""CRLF, s_config->ssid, s_config->passwd);
	return true;
}

static Step_t steps[] = {
		{ startRestartModule, NULL, NULL, STEP_INIT, 0 },
		{ startEchoOff, NULL, NULL, STEP_INIT, 0 },
		{ startModeChange, NULL, NULL, STEP_INIT, 0 },
		{ startConnect, NULL, NULL, STEP_INIT, 0 },
};

static char queryBuffer[1024];

static Request_t testReq = {
		0, QUERY_INIT,
		steps, sizeof(steps)/sizeof(*steps), 0,
		queryBuffer, sizeof(queryBuffer)
};

static USART_HandleTypeDef s_usart;

void QueryTest(void) {

	USART_deviceInit(&s_usart, 115200);
	do {
		QueryProcess(&testReq);
		HAL_USART_Transmit(&s_usart, testReq.buffer, testReq.bufferOccupied, 0xFF);
	} while ((testReq.state != QUERY_FAILED) && (testReq.state != QUERY_DONE));
}

void HAL_USART_RxCpltCallback(USART_HandleTypeDef *husart) {
	if (husart == &s_usart) {

	}
}
