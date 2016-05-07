/*
 * parsers.c
 *
 *  Created on: May 7, 2016
 *      Author: shapa
 */

#include "parsers.h"
#include "Query.h"
#include "memman.h"
#include <string.h>

#include "dbg_base.h"
#if 01
#include "dbg_trace.h"
#endif

static const char *const s_macPattern = "%x:%x:%x:%x:%x:%x";

_Bool Parse_APN(const char *buffer, size_t size, WIFI_APN_p *pApns, size_t *apnsCount) {
	_Bool result = false;
	static const char *tockDelim = ",\"";
	static const char *header = "+CWLAP:(";
	const size_t headerLen = strlen(header);

	do {
		size_t count = 0;
		char *ptr = (char*)buffer;
		char *apnSave = NULL;
		if (!buffer || !size || !pApns || !apnsCount) {
		    DBGMSG_M("Invalid input");
		    break;
		}
		while ((ptr < (buffer+size)) && (ptr = strstr(ptr, header))) {
			count++;
			ptr++;
		}
		if (!count) {
		    DBGMSG_M("[%s] not found in buff", header);
		    break;
		}
	    DBGMSG_M("Found [%d]", count);
		WIFI_APN_p apn = *pApns;
	    if (*apnsCount) {
	    	count = count > *apnsCount ? *apnsCount : count;
	    } else {
	    	apn = (WIFI_APN_p)MEMMAN_malloc(count*sizeof(WIFI_APN_t));
	    	if (!apn)
	    		break;
	    	*apnsCount = count;
	    }
		char *apnTok = strtok_r((char*)buffer, CRLF, &apnSave);
		size_t i = 0;
		while (apnTok && (i < count)) {
		    char *tok = apnTok + headerLen;
		    char *tokSave = apnTok + headerLen;
		    uint8_t *macPtr = (uint8_t*)&(apn[i].mac);
		    apn[i].mac = 0;
		    DBGMSG_M("apnTok [%s]", apnTok);
		    tok = strtok_r(tok, tockDelim, &tokSave);
			size_t j = 0;
			while (tok) {
			    DBGMSG_M("%d:%d tok [%s]", i, j, tok);
			    switch(j) {
					case 0:
						sscanf(tok, "%d", &(apn[i].enc));
						break;
					case 1:
						sscanf(tok, "%s", apn[i].ssid);
						break;
					case 2:
						sscanf(tok, "%d", &apn[i].rssi);
						break;
					case 3:
						sscanf(tok, s_macPattern, &macPtr[5], &macPtr[4], &macPtr[3], &macPtr[2], &macPtr[1], &macPtr[0]);
						break;
					case 4:
						sscanf(tok, "%d", &apn[i].channel);
						break;
			    }
			    tok = strtok_r(NULL, tockDelim, &tokSave);
			    j++;
			}
		    apnTok = strtok_r(NULL, CRLF, &apnSave);
		    i++;
		}
		*pApns = apn;
		result = !!i;
	} while (0);

    DBGMSG_M("%s", result ? "Ok" : "Fail");
	return result;
}

//+CIFSR:STAIP,"192.168.1.75"
//+CIFSR:STAMAC,"18:fe:34:9b:f3:a2"

_Bool Parse_IP(const char *buffer, size_t size, uint32_t *pIp, uint64_t *pMac) {
	_Bool result = false;
	static const char *tockDelim = ",\"";
	static const char *header = "+CIFSR:";
	static const char *ipHeader = "STAIP";
	static const char *macHeader = "STAMAC";

	const size_t headerLen = strlen(header);

	uint32_t ip = 0;
	uint64_t mac = 0;

	do {
		size_t count = 0;
		char *ptr = (char*)buffer;
		char *ipSave = NULL;
		if (!buffer || !size || !pIp || !pMac) {
		    DBGMSG_M("Invalid input");
		    break;
		}
		while ((ptr < (buffer+size)) && (ptr = strstr(ptr, header))) {
			count++;
			ptr++;
		}
		if (!count) {
		    DBGMSG_M("[%s] not found in buff", header);
		    break;
		}
	    DBGMSG_M("Found [%d]", count);
		char *ipTok = strtok_r((char*)buffer, CRLF, &ipSave);
		size_t i = 0;
		while (ipTok && (i < count)) {
		    char *tok = ipTok + headerLen;
		    char *tokSave = NULL;
		    DBGMSG_M("ipTok [%s]", ipTok);
		    tok = strtok_r(tok, tockDelim, &tokSave);
			while (tok && !(mac && ip)) {
			    DBGMSG_M("tok [%s]", tok);
			    if (!strcmp(tok, ipHeader)) {
				    int ipPtr[4];
				    tok = strtok_r(NULL, tockDelim, &tokSave);
					sscanf(tok, "%d.%d.%d.%d", &ipPtr[3], &ipPtr[2], &ipPtr[1], &ipPtr[0]);
					ip = ipPtr[0] | ipPtr[1]<<8 | ipPtr[2]<<16 | ipPtr[3]<<24;
			    } else if (!strcmp(tok, macHeader)) {
			    	int macPtr[6];
				    tok = strtok_r(NULL, tockDelim, &tokSave);
					sscanf(tok, s_macPattern, &macPtr[5], &macPtr[4], &macPtr[3], &macPtr[2], &macPtr[1], &macPtr[0]);
					mac = macPtr[0] | macPtr[1]<<8 | macPtr[2]<<16 | macPtr[3]<<24;
			    }
			    tok = strtok_r(NULL, tockDelim, &tokSave);
			}
			ipTok = strtok_r(NULL, CRLF, &ipSave);
		}
		*pIp = ip;
		*pMac = mac;
		result = !!(mac && ip);
	} while (0);

    DBGMSG_M("IP [%p] MAC %p", ip, mac);

    DBGMSG_M("%s", result ? "Ok" : "Fail");
	return result;
}
