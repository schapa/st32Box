//
// This file is part of the GNU ARM Eclipse distribution.
// Copyright (c) 2014 Liviu Ionescu.
//

// ----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "usartWrapper.h"
#include "canWrapper.h"
#include "pwmWrapper.h"
#include "trigger.h"
#include "misc.h"
#include "bsp.h"

#include "systemStatus.h"

int main(int argc, char* argv[]) {

	(void)argc;
	(void)argv;

	USART_HandleTypeDef traceUsart;
	CAN_HandleTypeDef can1Bus;
	HAL_StatusTypeDef initResult = HAL_ERROR;

	SystemStatus_set(INFORM_IDLE);
	SystemStatus_setLedControl(Led_Green_SetState);

	USART_tracerInit(&traceUsart);
	initResult = CAN_init(&can1Bus);
	InitTrigger(&can1Bus);

	printHelpMessage();
	dumpUsartTraceProps(&traceUsart);
	dumpCANProps(&can1Bus);
	if (initResult != HAL_OK)
		trace_printf("Can init failed [%d] with code [%d]\n\r", initResult, can1Bus.ErrorCode);

	PWM_Init();
extern void USB_GenericInit(void);
extern void QueryTest(void);
QueryTest();
USB_GenericInit();
//simpleTest();

	while (true) {

	}

	return 0;
}
