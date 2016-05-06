/*
 * memman.c
 *
 *  Created on: Apr 21, 2016
 *      Author: shapa
 */

#include "memman.h"
#include "stm32f4xx_hal.h"
#include <stdlib.h>
#include "dbg_base.h"

#if 0
#include "dbg_trace.h"
#endif

void *MEMMAN_malloc(size_t size) {
	DBG_ENTRY;
	__disable_irq();
	void *ptr = malloc(size);
	__enable_irq();
	DBGMSG_M("Alloc %p", ptr);
	DBG_EXIT;
	return ptr;
}


void MEMMAN_free(void *ptr) {
	DBG_ENTRY;
	__disable_irq();
	free (ptr);
	__enable_irq();
	DBGMSG_M("Free %p", ptr);
	DBG_EXIT;
}
