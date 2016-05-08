/*
 * connect.c
 *
 *  Created on: May 7, 2016
 *      Author: shapa
 */

#include "Query.h"
#include "requests.h"
#include "parsers.h"
#include "bsp.h"
#include <string.h>
#include <stddef.h>
#include "dbg_base.h"

#if 01
#include "dbg_trace.h"
#endif

static _Bool startConnect (Request_p req);
static _Bool getMyAdress (Request_p req);
static _Bool getMyAdressOk (Request_p req);

static char *s_ssid = NULL;
static char *s_passwd = NULL;

static const Step_t s_steps[] = {
	{ startConnect, 				NULL, 				NULL, 0, 20*BSP_TICKS_PER_SECOND, 0, NULL },
	{ getMyAdress, 					getMyAdressOk, 		NULL, 0, STEP_DEF_TOUT, 0, NULL },
};

Request_p Request_GetConnect(char *ssid, char *passwd) {
	static Request_t request = {
			QUERY_INIT,
			s_steps, sizeof(s_steps)/sizeof(*s_steps), 0,
			{ s_RequestTxBuffer, sizeof(s_RequestTxBuffer), 0 },
			{ NULL }
	};
	request.state = QUERY_INIT;
	request.stepCurrent = 0;
	s_ssid = ssid;
	s_passwd = passwd;
	return &request;
}

static _Bool startConnect (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CWJAP=\"%s\",\"%s\""CRLF, s_ssid, s_passwd);
	return true;
}

static _Bool getMyAdress (Request_p req) {
	char *buff = (char*)req->tx.buff;
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIFSR"CRLF);
	return true;
}

static _Bool getMyAdressOk (Request_p req) {
	systemConfig_p config = SystemConfig_get();
	Parse_IP(req->rx.buff, req->rx.occupied, config->selfIp, config->selfMac);
	DBGMSG_H("IP [%d.%d.%d.%d], MAC [%x:%x:%x:%x:%x:%x]", config->selfIp[3], config->selfIp[2], config->selfIp[1], config->selfIp[0],
			config->selfMac[5], config->selfMac[4], config->selfMac[3], config->selfMac[2], config->selfMac[1], config->selfMac[0]);
	return true;
}
