/*
 * memman.h
 *
 *  Created on: Apr 21, 2016
 *      Author: shapa
 */

#ifndef MEMMAN_H_
#define MEMMAN_H_

#include <stddef.h>

void *MEMMAN_malloc(size_t size);
void MEMMAN_free(void *ptr);

#endif /* MEMMAN_H_ */
