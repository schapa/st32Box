/*
 * bs_dbg.h
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#ifndef DBG_TRACE_H_
#define DBG_TRACE_H_

#include <system.h>
#include "diag/Trace.h"
#include "dbg_base.h"

#define DBGMSG_L_COLOR ANSI_ESC_COLOR_BLUE
#define DBGMSG_M_COLOR ANSI_ESC_COLOR_MAGENTA
#define DBGMSG_H_COLOR ANSI_ESC_COLOR_GREEN
#define DBGMSG_WARN_COLOR ANSI_ESC_COLOR_YELLOW
#define DBGMSG_ERR_COLOR ANSI_ESC_COLOR_RED

#undef DBGMSG_L
#undef DBGMSG_M
#undef DBGMSG_H
#undef DBGMSG_WARN
#undef DBGMSG_ERR
#undef DBG_ENTRY
#undef DBG_EXIT

#define DBGMSG_L(fmt, ...) \
	dbgmsg(DBGMSG_L_COLOR, "", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define DBGMSG_M(fmt, ...) \
	dbgmsg(DBGMSG_M_COLOR, "", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define DBGMSG_H(fmt, ...) \
	dbgmsg(DBGMSG_H_COLOR, "", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define DBGMSG_WARN(fmt, ...) \
	dbgmsg(DBGMSG_WARN_COLOR, "WARN", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
#define DBGMSG_ERR(fmt, ...) \
	dbgmsg(DBGMSG_ERR_COLOR, "ERR!", __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)

#define DBG_ENTRY DBGMSG_L("--> Enter")
#define DBG_EXIT DBGMSG_L("<-- Exit")

void dbgmsg(char *clr, char *siv, const char *file, const char *func, int line, char *fmt, ...);

#endif /* DBG_TRACE_H_ */
