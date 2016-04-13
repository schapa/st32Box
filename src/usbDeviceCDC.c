/*
 * usbDeviceCDC.c
 *
 *  Created on: Apr 13, 2016
 *      Author: shapa
 */


#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "diag/Trace.h"

static void freeHandle(PCD_HandleTypeDef *handle);

static PCD_HandleTypeDef *s_CDChandle;

/* USB PHY callback's */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}
void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd) {
	trace_printf("USB: [Callback] %s\n\r", __FUNCTION__);
}

void usbDeviceCDC_Init(void) {

	static const size_t deviceStackSize = 16*0x400;

	HAL_StatusTypeDef result = HAL_ERROR;
	PCD_InitTypeDef iFaceParams = {
			1, //Device Endpoints number.
			1, //Host Channels number.
			USB_OTG_SPEED_HIGH,
			DISABLE, // USB embedded DMA.
			DEP0CTL_MPS_64, // Endpoint 0 Max Packet size.
			USB_OTG_EMBEDDED_PHY,
			DISABLE, // output of the SOF signal.
			DISABLE, // low power mode.
			DISABLE, // Link Power Management.
			DISABLE, // Battery charging.
			ENABLE,  // VBUS Sensing feature.
			DISABLE, // dedicated EP1 interrupt
			DISABLE, // use of the external VBUS.
	};

	do {
		freeHandle(s_CDChandle);

		s_CDChandle = calloc(1, sizeof(PCD_HandleTypeDef));
		if (!s_CDChandle) {
			trace_printf("USB: [Error] Failed to allocate Instance [%zu]\n\r", sizeof(PCD_HandleTypeDef));
			break;
		}

		s_CDChandle->pData = malloc(deviceStackSize);
		if (!s_CDChandle->pData) {
			trace_printf("USB: [Error] Failed to allocate Stack [%zu]\n\r", deviceStackSize);
			break;
		}

		s_CDChandle->Instance = USB_OTG_FS;
		s_CDChandle->Init = iFaceParams;

		result = HAL_PCD_Init(s_CDChandle);
		if (result != HAL_OK) {
			trace_printf("USB: [Error] Failed to initialize USB [%d]\n\r", result);
			break;
		}

		result = HAL_PCD_Start(s_CDChandle);
		if (result != HAL_OK) {
			trace_printf("USB: [Error] Failed to start USB [%d]\n\r", result);
		}
	} while (false);

	if (result != HAL_OK) {
		freeHandle(s_CDChandle);
	}
}

static void freeHandle(PCD_HandleTypeDef *handle) {
	if (handle) {
		free(handle->pData);
		free(handle);
	}
}

/* interrupts */
void OTG_FS_WKUP_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_CDChandle);
}

void OTG_FS_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_CDChandle);
}


void OTG_HS_EP1_OUT_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_CDChandle);
}

void OTG_HS_EP1_IN_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_CDChandle);
}

void OTG_HS_WKUP_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_CDChandle);
}

void OTG_HS_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_CDChandle);
}
