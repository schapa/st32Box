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

_Bool Parse_APN(const char *buffer, size_t size, WIFI_APN_p *pApns, size_t *apnsCount) {
	_Bool result = false;
	static const char *tockDelim = ",\"";
	static const char *header = "+CWLAP:(";
	const size_t headerLen = strlen(header);
	static const char *patterns[] = {
			"%d",
			"%s",
			"%d",
			"%x:%x:%x:%x:%x:%x",
			"%d"
	};

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
						sscanf(tok, patterns[j], &(apn[i].enc));
						break;
					case 1:
						sscanf(tok, patterns[j], apn[i].ssid);
						break;
					case 2:
						sscanf(tok, patterns[j], &apn[i].rssi);
						break;
					case 3:
						sscanf(tok, patterns[j], &macPtr[0], &macPtr[1], &macPtr[2], &macPtr[3], &macPtr[4], &macPtr[5]);
//						TODO: check BE/LE
//						sscanf(tok, patterns[j], &macPtr[5], &macPtr[4], &macPtr[3], &macPtr[2], &macPtr[1], &macPtr[0]);
						break;
					case 4:
						sscanf(tok, patterns[j], &apn[i].channel);
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
