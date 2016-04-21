/*
 * canWrapper.c
 *
 *  Created on: Apr 2, 2016
 *      Author: shapa
 */


#include <string.h>
#include <stdlib.h>

#include "canWrapper.h"
#include "bsp.h"
#include "diag/Trace.h"

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
		free(handle->pRxMsg);
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

void HAL_CAN_TxCpltCallback(CAN_HandleTypeDef* hcan) {
	if (hcan && hcan->pTxMsg) {
		Event_t event = { EVENT_CAN, { ES_CAN_TX },
				.data.can = {
						hcan,
						.txMsg = *hcan->pTxMsg
				}
		};
		free(hcan->pTxMsg);
		BSP_queuePush(&event);
	}
}

void HAL_CAN_RxCpltCallback(CAN_HandleTypeDef* hcan) {
	if (hcan && hcan->pRxMsg) {
		Event_t event = { EVENT_CAN, { ES_CAN_RX },
				.data.can = {
						hcan,
						.rxMsg = *hcan->pRxMsg
				}
		};
		BSP_queuePush(&event);
		HAL_CAN_Receive_IT(hcan, CAN_FIFO0);
	}
}

void HAL_CAN_ErrorCallback(CAN_HandleTypeDef *hcan) {
	if (hcan) {
		Event_t event = { EVENT_CAN, { ES_CAN_ERROR },
				.data.can = {
						hcan,
						.txMsg = *hcan->pTxMsg
				}
		};
		BSP_queuePush(&event);
	}
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
