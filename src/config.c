/*
 * config.c
 *
 *  Created on: May 7, 2016
 *      Author: shapa
 */

#include "config.h"

static const credentials_t s_knownApns[] = {
		{ "shapa", "jnghfdkzq" }
};

static systemConfig_t s_defaultConfig = {
		NULL, 0,
		s_knownApns, sizeof(s_knownApns)/sizeof(*s_knownApns),
		8266,
		true
};

static systemConfig_p s_userConfig = NULL;

systemConfig_p SystemConfig_get(void) {
	if (!s_userConfig)
		s_userConfig = &s_defaultConfig;
	return s_userConfig;
}


