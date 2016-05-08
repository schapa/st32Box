/*
 * getRootXml.c
 *
 *  Created on: May 8, 2016
 *      Author: shapa
 */

#include "Query.h"
#include "requests.h"
#include "parsers.h"
#include "config.h"
#include "bsp.h"
#include <string.h>
#include <stddef.h>
#include "dbg_base.h"

#if 01
#include "dbg_trace.h"
#endif

static _Bool openTCP(Request_p req);
static _Bool prepareRoot(Request_p req);
static _Bool getRootDsc(Request_p req);
static _Bool getRootAck(Request_p req);

static const char *s_getRootStr = "GET /rootDesc.xml\r\n\r\n";

static const Step_t s_steps[] = {
	{ openTCP, 						NULL, 				NULL, 0, STEP_DEF_TOUT, 0, NULL },
	{ prepareRoot, 					NULL, 				NULL, 0, STEP_DEF_TOUT, 0, NULL },
	{ getRootDsc,		 			getRootAck, 		NULL, 0, STEP_DEF_TOUT, 0, "0,CLOSED" }
};

Request_p Request_GetRootXml(void) {
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

static _Bool openTCP(Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSTART=0,\"TCP\",\"192.168.1.1\",39201"CRLF);
	return true;
}
static _Bool prepareRoot(Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSEND=0,%d"CRLF, strlen(s_getRootStr));
	return true;
}
static _Bool getRootDsc(Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "%s"CRLF, s_getRootStr);
	return true;
}
static _Bool getRootAck(Request_p req) {
	DBGMSG_M("[Roo]\n\r%s", req->rx.buff);
	return true;
}


