/*
 * main.c
 *
 *  Created on: Apr 21, 2016
 *      Author: shapa
 */

#include "bsp.h"
#include "qpc.h"


int main(int argc, char* argv[]) {
	(void)argc;
	(void)argv;

	BSP_init();
    QF_init();

	return QF_run();
}
