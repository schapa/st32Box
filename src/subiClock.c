/*
 * subiClock.c
 *
 *  Created on: Mar 31, 2016
 *      Author: shapa
 */

#include <stdint.h>
#include "diag/Trace.h"
#include "stm32f4xx_hal.h"

#include "systemStatus.h"
#include "usartWrapper.h"
#include "misc.h"

typedef struct {
	uint8_t temperature; 		// 0 (temperature/2) - 40 °C or (0.9*temperature - 40) °F
	uint8_t brightness;  		// 1 [FF:00]
	/* For all consumptions
	 * 	0xFF -> empty line
	 * 	0xFE -> "---"
	 * 	0xFD -> Max value
	 */
	uint8_t consumptionCurrent; // 2 JDM/EDM (val/5 + 0.2) Liters. USDM (val/2 + 0.2) Gallons
	uint8_t consumptionA; 		// 3 JDM (val/10 + 0.1) km/L. EDM (val/5 + 0.2) L/100km. USDM (val*3/10) MPG
	uint8_t consumptionB; 		// 4 same as A
	uint8_t refuelDistance; 	// 5 (val*10 + 10) km/Miles
	struct {
		uint8_t maxBrightness:1; 	// 6 if 1 other brightness values are omitted. Brightness is locked
		uint8_t unlockBrightness:1; // 6 if 1 Brightness control is unlocked
		uint8_t bit2:1; 			// 6 TODO: UNKNOWN
		uint8_t disableDisp:1; 		// 6 block <Disp> button
		uint8_t consumptionState:2; // 6 00 - <Clock only> 01 - <A> 10 - <B> 11 - <Current>
		uint8_t isConfiguration:1;  // 6 if 1 - this is configure data
		uint8_t bit7:1; 			// 6 TODO: UNKNOWN
	} byte6;
	uint8_t byte7; 					// 7 TODO: UNKNOWN
	uint8_t byte8; 					// 8 bit0 - according to DISP. At startup [byte6;byte8] [0x54;0x00] [0x00;0x00]
	struct {
		uint8_t isKMpreLiter:1; 	// byte6 = 0x3С
		uint8_t isMPG:1;			// byte6 = 0x3С
		uint8_t isLper100km:2;		// byte6 = 0x3С
		uint8_t temperatureCorrection:2; // TODO: wtf?
		uint8_t isTemperatureBlinking:1;
		uint8_t sign:1;				// ? '-' : '+'
	} byte9;
	uint8_t byte10; 				// 10 if bit0 - true - setting clock
	uint8_t minutes; 				// 11
	uint8_t hours; 					// 12
	uint8_t byte13; 				// 13 TODO: UNKNOWN
	uint8_t crc; 					// 14 Sum of all
} subiClockMessage_t;

//static uint8_t s_testingSequence[15] = {
//		0x3D, 0xF0, 0xF0, 0xF0, 0xF0,
//		0x20, 0x3C, 0x00, 0x00, 0x04,
//		0xFC, 0x3F, 0x1F, 0x00, 0x00
//};
//static uint8_t s_testingSequence[15] = {
//		0xfE, 0x55, 0xfe, 0x22, 0x44,
//		0x23, 0x3C, 0x00, 0x00, 0x04,
//		0x00, 0x00, 0x00, 0x00, 0x9B
//};
static uint8_t s_testingSequence[15] = {
		0xFD, 0xff, 0xff, 0xff, 0xff,
		0xff, 0x00, 0x00, 0xff, 0xff,
		0xff, 0xff, 0xff, 0xff, 0xff
};
void simpleTest(void) {
	uint8_t summ = 0;
	uint8_t i;
	for (i = 0; i < 14; i++) {
		s_testingSequence[i] = i;
		summ += s_testingSequence[i];
	}
	s_testingSequence[14] = summ;
	trace_printf("summ [%x]\n\r", summ);

	USART_HandleTypeDef usart;
	USART_deviceInit(&usart, 9600);
	HELP_dumpUsartProps(&usart);
	while (1) {
		HAL_StatusTypeDef status = HAL_USART_Transmit(&usart, s_testingSequence, 15, 0xFF);
		trace_printf("tr [%d]\n\r", status);
		SystemTimer_delayMsDummy(100);
	}
}
