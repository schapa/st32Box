/*
 * memman.c
 *
 *  Created on: Apr 21, 2016
 *      Author: shapa
 */

#include "memman.h"
#include "stm32f4xx_hal.h"
#include <stdlib.h>

void *MEMMAN_malloc(size_t size) {
	__disable_irq();
	void *ptr = malloc(size);
	__enable_irq();
	return ptr;
}


void MEMMAN_free(void *ptr) {

	__disable_irq();
	free (ptr);
	__enable_irq();
}
