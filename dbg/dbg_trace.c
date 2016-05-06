/*
 * dbg_trace.c
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#include "systemStatus.h"
#include "diag/Trace.h"
#include "dbg_trace.h"
#include <stdarg.h>

static char s_msgBuffer[1024];
static int s_msgBufferSize = sizeof(s_msgBuffer);

static inline void timestamp(void){
	trace_printf("[%4d.%03d] ", System_getUptime(), System_getUptimeMs());
}

void dbgmsg(char *clr, char *siv, const char *file, const char *func, int line, char *fmt, ...) {
	timestamp();
	trace_printf("%s::%s (%d)", file, func, line);
	trace_printf("%s", clr);
	trace_printf("%s: ", siv);
	va_list ap;
	va_start (ap, fmt);

	int ret = vsnprintf (s_msgBuffer, s_msgBufferSize, fmt, ap);
	ret = ret > s_msgBufferSize ? s_msgBufferSize : ret;

	if (ret > 0) {
		ret = trace_write (s_msgBuffer, (size_t)ret);
	}
	va_end (ap);
	trace_printf(ANSI_ESC_DEFAULT"\r\n");
}
