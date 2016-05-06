/*
 * dbg_trace.c
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#include "usartWrapper.h"
#include "dbg_trace.h"
#include "system.h"
#include "memman.h"
#include <stdarg.h>
#include <string.h>

static char s_msgBuffer[4096];
static int s_msgBufferSize = sizeof(s_msgBuffer);

void dbgmsg(char *color, char *siverity, const char *file, const char *func, int line, char *fmt, ...) {
	uint32_t primask = __get_PRIMASK();
	__disable_irq();

	int occupied = snprintf(s_msgBuffer, s_msgBufferSize, "[%4lu.%03lu] %s::%s (%d)%s %s: ",
			System_getUptime(), System_getUptimeMs(), file, func, line, color, siverity);
	if (occupied < s_msgBufferSize) {
		va_list ap;
		va_start (ap, fmt);
		occupied += vsnprintf(&s_msgBuffer[occupied], s_msgBufferSize - occupied, fmt, ap);
		va_end (ap);
	}
	if (occupied < s_msgBufferSize) {
		occupied += snprintf(&s_msgBuffer[occupied], s_msgBufferSize - occupied, ANSI_ESC_DEFAULT"\r\n");
	}
	if (occupied > s_msgBufferSize) {
		char *trim = "...";
		size_t size = strlen(trim) + 1;
		snprintf(&s_msgBuffer[s_msgBufferSize-size], size, trim);
	}
	char *newBuff = (char*)MEMMAN_malloc(occupied);
	if (newBuff) {
		memcpy((void*)newBuff, (void*)s_msgBuffer, occupied);
	}
	if (!primask) {
		__enable_irq();
	}
	Trace_dataAsync(newBuff, occupied);
}
