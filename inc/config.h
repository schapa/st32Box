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
	WIFI_APN_p discoveredApns;
	size_t discoveredApnsSize;

	credentials_p *knownApns;
	size_t knownApnsCount;

	uint16_t externalPort;
	_Bool isUPnPActive;
} systemConfig_t, *systemConfig_p;

systemConfig_p SystemConfig_get(void);


#endif /* CONFIG_H_ */
