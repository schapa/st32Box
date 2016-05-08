/*
 * requests.h
 *
 *  Created on: May 7, 2016
 *      Author: shapa
 */

#ifndef REQUESTS_H_
#define REQUESTS_H_

#include "Query.h"

char s_RequestTxBuffer[1024];

Request_p Request_GetInitial(void);
Request_p Request_GetConnect(char *ssid, char *passwd);
Request_p Request_GetUPnPDiscoverer(void);
Request_p Request_GetRootXml(void);
Request_p Request_GetCheckConnection(void);
Request_p Request_GetExternalIp(void);


extern const char *s_httpPostSoap;

#endif /* REQUESTS_H_ */
