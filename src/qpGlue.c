/*
 * qpGlue.c
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#include "qpc.h"
#include "dbg_trace.h"

void Q_onAssert(char_t const * const module, int_t location) {
	DBGMSG_ERR("%s : %d", module, location);
}

void QF_onStartup(void) {
	DBGMSG_H("...");
}

void QV_onIdle(void) {
    QF_INT_ENABLE();
}
