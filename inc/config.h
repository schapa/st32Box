/*
 * config.h
 *
 *  Created on: May 7, 2016
 *      Author: shapa
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#include "parsers.h"

typedef struct {
	char ssid[33];
	char passwd[64];
} credentials_t, *credentials_p;

typedef struct {
	credentials_p knownApns;
	size_t knownApnsCount;

	uint16_t externalPort;
} nonVolatileSettings_t, *nonVolatileSettings_p;

typedef struct {
	WIFI_APN_p discoveredApns;
	size_t discoveredApnsSize;

	nonVolatileSettings_p settings;

	uint8_t selfIp[4];
	uint8_t selfMac[6];

	struct {
		_Bool isUPnPActive;
		_Bool isConnected;
	    uint8_t ip[4];
	    uint16_t port;
	    char rootUrl[64];
	    char ctrlUrl[64];
	} igd;
} systemConfig_t, *systemConfig_p;

systemConfig_p SystemConfig_get(void);


#endif /* CONFIG_H_ */
