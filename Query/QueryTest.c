/*
 * QueryTest.c
 *
 *  Created on: Apr 19, 2016
 *      Author: shapa
 */

#include "Query.h"
#include "stm32f4xx_hal.h"

#include "bsp.h"
#include "uartWrapper.h"
#include "memman.h"
#include <string.h>
#include "dbg_base.h"

#if 01
#include "dbg_trace.h"
#endif

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
	DBGMSG_M("[APS]\n\r%s", req->rx.buff);
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
	DBGMSG_M("[IP]\n\r%s", req->rx.buff);
	return true;
}

static _Bool setMultipleMode (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPMUX=1"CRLF);
	return true;
}

static _Bool openUpdBroadcast (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSTART=0,\"UDP\",\"239.255.255.250\",1900"CRLF);
	return true;
}

char *upnpDiscoverer = "M-SEARCH * HTTP/1.1\r\nHOST:239.255.255.250:1900\r\nMAN:\"ssdp:discover\"\r\nST:ssdp:all\r\nMX:3\r\n\r\n";

static _Bool prepareToSend (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSEND=0,%d"CRLF, strlen(upnpDiscoverer));
	return true;
}

static _Bool sendUPnPDiscover (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "%s"CRLF, upnpDiscoverer);
	return true;
}
static _Bool uPnPDiscoverAck (Request_p req) {
	DBGMSG_M("[UPnP]\n\r%s", req->rx.buff);
	return true;
}
static _Bool closeUpdBroadcast (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPCLOSE=0"CRLF);
	return true;
}

static _Bool openTCP (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSTART=0,\"TCP\",\"192.168.1.1\",39201"CRLF);
	return true;
}
char *getRoo = "GET /rootDesc.xml\r\n\r\n";
static _Bool prepareRoot (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSEND=0,%d"CRLF, strlen(getRoo));
	return true;
}
static _Bool getRootDsc (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "%s"CRLF, getRoo);
	return true;
}
static _Bool getRootAck (Request_p req) {
	DBGMSG_M("[Roo]\n\r%s", req->rx.buff);
	return true;
}


static Step_t steps[] = {
	{ startRestartModule, 			NULL, 				NULL, 0, 0, "ready" },
	{ startEchoOff, 				NULL, 				NULL, 0, 0, NULL },
	{ startModeChange, 				NULL, 				NULL, 0, 0, NULL },
	{ listAccessPoints, 			listAccessPointsOk, NULL, 0, 0, NULL },
	{ startConnect, 				NULL, 				NULL, 0, 0, NULL },
	{ getMyAdress, 					getMyAdressOk, 		NULL, 0, 0, NULL },
	{ setMultipleMode, 				NULL, 				NULL, 0, 0, NULL },
	{ openUpdBroadcast, 			NULL, 				NULL, 0, 0, NULL },
	{ prepareToSend, 				NULL, 				NULL, 0, 0, "> " },
	{ sendUPnPDiscover, 			NULL, 				NULL, 0, 0, "SEND OK" },
	{ NULL, 						uPnPDiscoverAck, 	NULL, STEP_FLAG_WAIT_TOUT, 1*BSP_TICKS_PER_SECOND, NULL },
	{ closeUpdBroadcast, 			NULL, 				NULL, 0, 0, NULL },
	{ openTCP, 						NULL, 				NULL, 0, 0, NULL },
	{ prepareRoot, 					NULL, 				NULL, 0, 0, NULL },
	{ getRootDsc,		 			getRootAck, 		NULL, 0, 0, "0,CLOSED" },
};

static char queryBuffer[1024];
static UART_HandleTypeDef s_uart;

Request_t testReq = {
		QUERY_INIT,
		steps, sizeof(steps)/sizeof(*steps), 0,
		{ queryBuffer, sizeof(queryBuffer), 0 }
};

