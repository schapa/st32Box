/*
 * canWrapper.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */

#include "canWrapper.h"

#include "diag/Trace.h"
#include <string.h>
#include <stdlib.h>

static CAN_HandleTypeDef *s_can1Handle = NULL;

HAL_StatusTypeDef CAN_init(CAN_HandleTypeDef *handle) {

	HAL_StatusTypeDef result = HAL_ERROR;
	const uint32_t baudRate = 125000;

	CAN_InitTypeDef ifaceParams = {
			HAL_RCC_GetPCLK1Freq()/(baudRate*(1+4+3)), //(CAN_SJW + CAN_BS1 + CAN_BS2)
			CAN_MODE_NORMAL,
			CAN_SJW_1TQ,
			CAN_BS1_4TQ,
			CAN_BS2_3TQ,
			.TTCM = DISABLE,
			.ABOM = DISABLE,
			.AWUM = DISABLE,
			.NART = DISABLE,
			.RFLM = DISABLE,
			.TXFP = DISABLE,
	};
	CAN_FilterConfTypeDef filter = {
			0, 0, 0, 0,
			CAN_FIFO0,
			0,
			CAN_FILTERMODE_IDMASK,
			CAN_FILTERSCALE_32BIT,
			ENABLE,
			0
	};

	if (handle) {
		memset(handle, 0, sizeof(*handle));
		handle->Instance = CAN1;
		handle->Init = ifaceParams;

		result = HAL_CAN_Init(handle);
		if (result == HAL_OK) {
			HAL_CAN_ConfigFilter(handle, &filter);
			HAL_NVIC_EnableIRQ(CAN1_TX_IRQn);
			HAL_NVIC_EnableIRQ(CAN1_RX0_IRQn);
			HAL_NVIC_EnableIRQ(CAN1_RX1_IRQn);
//			HAL_NVIC_EnableIRQ(CAN1_SCE_IRQn);
			handle->pRxMsg = malloc(sizeof(*handle->pRxMsg));
			HAL_CAN_Receive_IT(handle, CAN_FIFO0);
		}
		s_can1Handle = handle;
	}
	return result;
}



void CAN1_TX_IRQHandler(void) {
	HAL_CAN_IRQHandler(s_can1Handle);
}
void CAN1_RX0_IRQHandler(void) {
	HAL_CAN_IRQHandler(s_can1Handle);
}
void CAN1_RX1_IRQHandler(void) {
	HAL_CAN_IRQHandler(s_can1Handle);
}
void CAN1_SCE_IRQHandler(void) {
	HAL_CAN_IRQHandler(s_can1Handle);
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan) {

	uint32_t id = hcan->pRxMsg->IDE ? hcan->pRxMsg->ExtId : hcan->pRxMsg->StdId;
	if (hcan->pRxMsg->RTR) {
		trace_printf("RTR [%p] \n\r", id);
	} else {
		trace_printf("DLC [%p] = [", id);
		for (uint32_t i = 0; i < hcan->pRxMsg->DLC; i++)
			trace_printf(" %d", hcan->pRxMsg->Data[i]);
		trace_printf("] = %d\n\r", hcan->pRxMsg->DLC);
	}

	HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
}
