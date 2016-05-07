/*
 * QueryTest.c
 *
 *  Created on: Apr 19, 2016
 *      Author: shapa
 */

#include "Query.h"
#include "requests.h"
#include "parsers.h"
#include "config.h"
#include <string.h>
#include <stddef.h>
#include "dbg_base.h"

#if 01
#include "dbg_trace.h"
#endif

static _Bool restartModule (Request_p req);
static _Bool echoOff (Request_p req);
static _Bool modeChange (Request_p req);
static _Bool listAccessPoints (Request_p req);
static _Bool parseAccessPointsList (Request_p req);

static Step_t s_steps[] = {
	{ restartModule, 	NULL, 					NULL, 0, 0, "ready" },
	{ echoOff, 			NULL, 					NULL, 0, 0, NULL },
	{ modeChange, 		NULL, 					NULL, 0, 0, NULL },
	{ listAccessPoints, parseAccessPointsList, 	NULL, 0, 0, NULL },
};

Request_p Request_GetInitial(void) {
	static Request_t request = {
			QUERY_INIT,
			s_steps, sizeof(s_steps)/sizeof(*s_steps), 0,
			{ s_RequestTxBuffer, sizeof(s_RequestTxBuffer), 0 },
			{ NULL }
	};

	request.state = QUERY_INIT;
	request.stepCurrent = 0;
	return &request;
}

static _Bool restartModule (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+RST"CRLF);
	return true;
}

static _Bool echoOff (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "ATE0"CRLF);
	return true;
}

static _Bool modeChange (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CWMODE=1"CRLF);
	return true;
}

static _Bool listAccessPoints (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CWLAP"CRLF);
	return true;
}
static _Bool parseAccessPointsList (Request_p req) {
	systemConfig_p config = SystemConfig_get();
	Parse_APN(req->rx.buff, req->rx.occupied, &config->discoveredApns, &config->discoveredApnsSize);
	if (config->discoveredApnsSize) {
		DBGMSG_M("Discovered [%d] APNs", config->discoveredApnsSize);
		size_t i;
		for (i = 0; i < config->discoveredApnsSize; i++) {
			DBGMSG_L("%d: [%s] chn %d rssi %d", i, config->discoveredApns[i].ssid,
					config->discoveredApns[i].channel, config->discoveredApns[i].rssi);
		}
	} else {
		DBGMSG_H("No APN's Found");
	}
	return true;
}

