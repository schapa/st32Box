/*
 * pwmWrapper.c
 *
 *  Created on: Apr 7, 2016
 *      Author: pavelgamov
 */

#include "diag/Trace.h"
#include "stm32f4xx_hal.h"
#include "pwmWrapper.h"

static void timerBaseInit(TIM_HandleTypeDef *htim);
static void timerOscInit(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t dutPercent);

static TIM_HandleTypeDef s_tim1Handle;

void PWM_Init(void) {

	timerBaseInit(&s_tim1Handle);
	PWM_Configure(TIM_CHANNEL_1, 0);
}

void PWM_Configure(uint32_t channel, uint8_t dutPercent) {

	timerOscInit(&s_tim1Handle, channel, dutPercent);
	HAL_TIM_PWM_Start(&s_tim1Handle, TIM_CHANNEL_1);
}

static void timerBaseInit(TIM_HandleTypeDef *htim) {

	if (!htim) {
		return;
	}

	TIM_Base_InitTypeDef iface = {
			0xFFF,
			TIM_COUNTERMODE_UP,
			0xFF,
			TIM_CLOCKDIVISION_DIV4,
			0
	};
	htim->Init = iface;
	htim->Instance = TIM1;
	HAL_TIM_PWM_Init(htim);
}

static void timerOscInit(TIM_HandleTypeDef *htim, uint32_t channel, uint8_t dutPercent) {

	if (!htim || (dutPercent > 100)) {
		return;
	}
	uint16_t dutCycle = htim->Init.Period*dutPercent/100;
	TIM_OC_InitTypeDef sConfigOC = {
			TIM_OCMODE_PWM1,
			dutCycle,
			TIM_OCPOLARITY_HIGH,
			TIM_OCNPOLARITY_LOW,
			TIM_OCFAST_DISABLE,
			TIM_OCIDLESTATE_RESET,
			TIM_OCNIDLESTATE_SET
	};
	HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, channel);
}
