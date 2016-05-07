/*
 * config.c
 *
 *  Created on: May 7, 2016
 *      Author: shapa
 */

#include "config.h"

static systemConfig_t s_defaultConfig = {
		NULL, 0,
		{ { "a", "b" } }, 1,
		8266,
		true
};

static systemConfig_p s_userConfig = NULL;

systemConfig_p SystemConfig_get(void) {
	if (!s_userConfig)
		s_userConfig = &s_defaultConfig;
	return s_userConfig;
}


