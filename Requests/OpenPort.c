/*
 * OpenPort.c
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
static _Bool preparePacket(Request_p req);
static _Bool sendPacket(Request_p req);
static _Bool packetAck(Request_p req);

static _Bool openListen(Request_p req) {
	char *buff = (char*)req->tx.buff;
	systemConfig_p config = SystemConfig_get();
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSERVER=1,8266"CRLF);
	return true;
}

static const char *s_soapAction = "urn:schemas-upnp-org:service:WANIPConnection:1#AddPortMapping";
static const char *s_addPortMapping =
	"<?xml version=\"1.0\"?>"CRLF
	"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
	"<s:Body>"
	"<u:AddPortMapping xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\">"
	"<NewRemoteHost></NewRemoteHost>"
	"<NewExternalPort>8266</NewExternalPort>"
	"<NewProtocol>TCP</NewProtocol>"
	"<NewInternalPort>8266</NewInternalPort>"
	"<NewInternalClient>192.168.1.75</NewInternalClient>"
	"<NewEnabled>1</NewEnabled>"
	"<NewPortMappingDescription>libminiupnpc</NewPortMappingDescription>"
	"<NewLeaseDuration>0</NewLeaseDuration>"
	"</u:AddPortMapping>"
	"</s:Body>"
	"</s:Envelope>"CRLF;

static const Step_t s_steps[] = {
	{ openTCP, 						NULL, 				NULL, 0, STEP_DEF_TOUT, 0, NULL },
	{ preparePacket, 				NULL, 				NULL, 0, STEP_DEF_TOUT, 0, NULL },
	{ sendPacket,		 			NULL, 				NULL, 0, STEP_DEF_TOUT, 0, "SEND OK" },
	{ NULL,				 			packetAck, 			NULL, 0, STEP_DEF_TOUT, 0, "0,CLOSED" },
	{ openListen,				 			NULL, 			NULL, 0, STEP_DEF_TOUT, 0, NULL }
};

Request_p Request_GetOpenPort(void) {
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
	systemConfig_p config = SystemConfig_get();
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSTART=0,\"TCP\",\"%d.%d.%d.%d\",%d"CRLF,
			config->igd.ip[3], config->igd.ip[2], config->igd.ip[1], config->igd.ip[0], config->igd.port);
	return true;
}
static _Bool preparePacket(Request_p req) {
	char *buff = (char*)req->tx.buff;
	systemConfig_p config = SystemConfig_get();
	size_t len = snprintf(buff, req->tx.size, s_httpPostSoap,
			config->igd.ctrlUrl,
			config->igd.ip[3], config->igd.ip[2], config->igd.ip[1], config->igd.ip[0], config->igd.port,
			strlen(s_addPortMapping),
			s_soapAction,
			s_addPortMapping);
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSEND=0,%d"CRLF, len);
	return true;
}
static _Bool sendPacket(Request_p req) {
	char *buff = (char*)req->tx.buff;
	systemConfig_p config = SystemConfig_get();
	req->tx.occupied = snprintf(buff, req->tx.size, s_httpPostSoap,
			config->igd.ctrlUrl,
			config->igd.ip[3], config->igd.ip[2], config->igd.ip[1], config->igd.ip[0], config->igd.port,
			strlen(s_addPortMapping),
			s_soapAction,
			s_addPortMapping);
	return true;
}
static _Bool packetAck(Request_p req) {
//	systemConfig_p config = SystemConfig_get();
	DBGMSG_H("\r\n[%s]", req->rx.buff);
	return true;
}
