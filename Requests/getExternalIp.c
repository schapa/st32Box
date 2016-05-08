/*
 * getExterrnalIp.c
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

static const char *s_soapAction = "urn:schemas-upnp-org:service:WANIPConnection:1#GetExternalIPAddress";
static const char *s_getExtIpEnvelope =
		"<?xml version=\"1.0\"?>"CRLF
		"<s:Envelope xmlns:s=\"http://schemas.xmlsoap.org/soap/envelope/\" s:encodingStyle=\"http://schemas.xmlsoap.org/soap/encoding/\">"
		"<s:Body>"
		"<u:GetExternalIPAddress xmlns:u=\"urn:schemas-upnp-org:service:WANIPConnection:1\"></u:GetExternalIPAddress>"
		"</s:Body>"
		"</s:Envelope>"CRLF;

static const Step_t s_steps[] = {
	{ openTCP, 						NULL, 				NULL, 0, STEP_DEF_TOUT, 0, NULL },
	{ preparePacket, 				NULL, 				NULL, 0, STEP_DEF_TOUT, 0, NULL },
	{ sendPacket,		 			NULL, 				NULL, 0, STEP_DEF_TOUT, 0, "SEND OK" },
	{ NULL,				 			packetAck, 			NULL, 0, STEP_DEF_TOUT, 0, "0,CLOSED" }
};

Request_p Request_GetExternalIp(void) {
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
			strlen(s_getExtIpEnvelope),
			s_soapAction,
			s_getExtIpEnvelope);
	req->tx.occupied = snprintf(buff, req->tx.size, "AT+CIPSEND=0,%d"CRLF, len);
	return true;
}
static _Bool sendPacket(Request_p req) {
	char *buff = (char*)req->tx.buff;
	systemConfig_p config = SystemConfig_get();
	req->tx.occupied = snprintf(buff, req->tx.size, s_httpPostSoap,
			config->igd.ctrlUrl,
			config->igd.ip[3], config->igd.ip[2], config->igd.ip[1], config->igd.ip[0], config->igd.port,
			strlen(s_getExtIpEnvelope),
			s_soapAction,
			s_getExtIpEnvelope);
	return true;
}
static _Bool packetAck(Request_p req) {
	DBGMSG_H(CRLF"%s", req->rx.buff);
//	systemConfig_p config = SystemConfig_get();
//	config->igd.isUPnPActive &= Parse_RootXML(req->rx.buff, req->rx.occupied, config->igd.ctrlUrl);
	return true;
}

