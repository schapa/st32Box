/*
 * upnpdiscoverer.c
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

static _Bool setMultipleMode (Request_p req);
static _Bool openUpdBroadcast (Request_p req);
static _Bool prepareToSend (Request_p req);
static _Bool sendUPnPDiscover (Request_p req);
static _Bool uPnPDiscoverAck (Request_p req);
static _Bool closeUpdBroadcast (Request_p req);

static const char *upnpDiscoverer = "M-SEARCH * HTTP/1.1\r\nHOST:239.255.255.250:1900\r\nMAN:\"ssdp:discover\"\r\nST:ssdp:all\r\nMX:3\r\n\r\n";

static Step_t s_steps[] = {
	{ setMultipleMode, 				NULL, 				NULL, 0, 0, NULL },
	{ openUpdBroadcast, 			NULL, 				NULL, 0, 0, NULL },
	{ prepareToSend, 				NULL, 				NULL, 0, 0, "> " },
	{ sendUPnPDiscover, 			NULL, 				NULL, 0, 0, "SEND OK" },
	{ NULL, 						uPnPDiscoverAck, 	NULL, STEP_FLAG_WAIT_TOUT, 1*BSP_TICKS_PER_SECOND, NULL },
	{ closeUpdBroadcast, 			NULL, 				NULL, 0, 0, NULL },
};

Request_p Request_GetUPnPDiscoverer(void) {
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

