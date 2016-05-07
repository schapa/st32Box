/*
 * qpGlue.c
 *
 *  Created on: May 6, 2016
 *      Author: shapa
 */

#include "qpc.h"
#include "dbg_trace.h"
#include "Engine.h"
#include "QueryEngine.h"

static QF_MPOOL_EL(QTimeEvt) smlPoolSto[20];
static QF_MPOOL_EL(SystemEvent) sysPoolSto[256];

void Q_onAssert(char_t const * const module, int_t location) {
	DBGMSG_ERR("%s : %d", module, location);
	while (1);
}

void QF_onStartup(void) {
	DBGMSG_H("...");
    Engine *eng = (Engine*)AO_system();
    QueryEngine *query = (QueryEngine*)AO_QueryEngine();

    QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));
    QF_poolInit(sysPoolSto, sizeof(sysPoolSto), sizeof(sysPoolSto[0]));

    QACTIVE_START(AO_QueryEngine(),
                  2U,             /* unique QP priority of the AO */
				  query->evtStorage,   /* storage for the AO's queue */
                  Q_DIM(query->evtStorage), /* lenght of the queue [entries] */
                  (void *)0, 0U, (QEvt *)0);

    QACTIVE_START(AO_system(),
                  1U,             /* unique QP priority of the AO */
				  eng->evtStorage,   /* storage for the AO's queue */
                  Q_DIM(eng->evtStorage), /* lenght of the queue [entries] */
                  (void *)0, 0U, (QEvt *)0);
}

void QV_onIdle(void) {
    QF_INT_ENABLE();
}
