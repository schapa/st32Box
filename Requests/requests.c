/*
 * requests.c
 *
 *  Created on: May 8, 2016
 *      Author: shapa
 */

#include "requests.h"

const char *s_httpPostSoap =
		"POST %s HTTP/1.1"CRLF
		"Host: %d.%d.%d.%d:%d"CRLF
		"User-Agent: Debian/jessie/sid, UPnP/1.0, MiniUPnPc/1.6"CRLF
		"Content-Length: %d"CRLF
		"Content-Type: text/xml"CRLF
		"SOAPAction: \"%s\""CRLF
		"Connection: Close"CRLF
		"Cache-Control: no-cache"CRLF
		"Pragma: no-cache\r\n"CRLF
		"%s";

