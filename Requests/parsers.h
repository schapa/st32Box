/*
 * parsers.h
 *
 *  Created on: May 7, 2016
 *      Author: shapa
 */

#ifndef PARSERS_H_
#define PARSERS_H_

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

typedef enum {
	WIFI_ENC_OPEN = 0,
	WIFI_ENC_WEP,
	WIFI_ENC_WPA_PSK,
	WIFI_ENC_WPA2_PSK,
	WIFI_ENC_WPA_WPA2_PSK
} WIFI_encryption_t;

typedef struct {
	WIFI_encryption_t enc;
	char ssid[33]; // IEEE 802.11-2007 = 32 + '\0'
	int8_t rssi; // abs (0 - 100)
	uint8_t mac[6];
	uint8_t channel;
} WIFI_APN_t, *WIFI_APN_p;

/** @brief parse buffer for APN's
 * return true on success
 */
_Bool Parse_APN(const char *buffer, size_t size, WIFI_APN_p *pApns, size_t *apnsCount);
_Bool Parse_IP(const char *buffer, size_t size, uint8_t *pIp, uint8_t *pMac);

#endif /* PARSERS_H_ */
