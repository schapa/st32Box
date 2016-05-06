/*
 * main.c
 *
 *  Created on: Apr 21, 2016
 *      Author: shapa
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "stm32f4xx_hal.h"

#include "bsp.h"
#include "USB_Generic.h"
#include "uartWrapper.h"
#include "canWrapper.h"
#include "misc.h"
#include "timers.h"
#include "Query.h"
#include "qpc.h"
#include "QueryEngine.h"

static QF_MPOOL_EL(QTimeEvt) smlPoolSto[20];
static QF_MPOOL_EL(SystemEvent) sysPoolSto[256];

int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

	BSP_init();
    QF_init();

    QF_poolInit(smlPoolSto, sizeof(smlPoolSto), sizeof(smlPoolSto[0]));
    QF_poolInit(sysPoolSto, sizeof(sysPoolSto), sizeof(sysPoolSto[0]));

    QueryEngine *eng = (QueryEngine*)AO_system();

    QACTIVE_START(AO_system(),
                  1U,             /* unique QP priority of the AO */
				  eng->evtStorage,   /* storage for the AO's queue */
                  Q_DIM(eng->evtStorage), /* lenght of the queue [entries] */
                  (void *)0, 0U, (QEvt *)0);

	return QF_run();
}
