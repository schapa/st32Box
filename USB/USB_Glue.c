/*
 * USB_Glue.c
 *
 *  Created on: Apr 13, 2016
 *      Author: shapa
 */


#include "stm32f4xx_hal.h"
#include "diag/Trace.h"
#include "usbd_core.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

static void initUSB_NVIC(void);

USBD_StatusTypeDef USBD_LL_Init (USBD_HandleTypeDef *pDev) {

	PCD_HandleTypeDef *handle = pDev ? pDev->pData : NULL;
	_Bool isHighSpeed = true; //pDev->id == DEVICE_HS
	PCD_InitTypeDef iface = {
			7, // Device Endpoints number.
			1, // Host Channels number.
			isHighSpeed ? USB_OTG_SPEED_HIGH : USB_OTG_SPEED_FULL,
			DISABLE, // USB embedded DMA.
			DEP0CTL_MPS_64, // Endpoint 0 Max Packet size.
			USB_OTG_EMBEDDED_PHY,
			DISABLE, // output of the SOF signal.
			DISABLE, // low power mode.
			DISABLE, // Link Power Management.
			DISABLE, // Battery charging.
			DISABLE, // VBUS Sensing feature.
			DISABLE, // use of the dedicated EP1 interrupt.
			DISABLE, // use of the external VBUS.
	};

	if (!pDev)
		return USBD_FAIL;

	free(handle);
	handle = (PCD_HandleTypeDef*)malloc(sizeof(PCD_HandleTypeDef));
	if (!handle)
		return USBD_FAIL;
	handle->Init = iface;
	handle->Instance = isHighSpeed ? USB_OTG_HS : USB_OTG_FS;
	handle->pData = (void*)pDev;
	pDev->pData = (void*)handle;

	HAL_PCD_Init(handle);
	initUSB_NVIC();
	HAL_PCD_SetRxFiFo(handle, 0x80);
	HAL_PCD_SetTxFiFo(handle, 0, 0x40);
	HAL_PCD_SetTxFiFo(handle, 1, 0x80);

	return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_DeInit(USBD_HandleTypeDef *pdev) {
	HAL_PCD_DeInit((PCD_HandleTypeDef*)pdev->pData);
	free(pdev);
	return USBD_OK;
}

USBD_StatusTypeDef  USBD_LL_Start(USBD_HandleTypeDef *pdev) {
	HAL_PCD_Start((PCD_HandleTypeDef*)pdev->pData);
	return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_Stop(USBD_HandleTypeDef *pdev) {
	HAL_PCD_Stop((PCD_HandleTypeDef*)pdev->pData);
	return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_OpenEP(USBD_HandleTypeDef *pdev, uint8_t  ep_addr, uint8_t  ep_type, uint16_t ep_mps) {
	HAL_PCD_EP_Open((PCD_HandleTypeDef*)pdev->pData, ep_addr, ep_mps, ep_type);
	return USBD_OK;
}

USBD_StatusTypeDef  USBD_LL_CloseEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_Close((PCD_HandleTypeDef*)pdev->pData, ep_addr);
	return USBD_OK;
}

USBD_StatusTypeDef  USBD_LL_FlushEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_Flush((PCD_HandleTypeDef*)pdev->pData, ep_addr);
	return USBD_OK;
}

USBD_StatusTypeDef  USBD_LL_StallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_SetStall((PCD_HandleTypeDef*)pdev->pData, ep_addr);
	return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_ClearStallEP(USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	HAL_PCD_EP_ClrStall((PCD_HandleTypeDef*)pdev->pData, ep_addr);
	return USBD_OK;
}

uint8_t USBD_LL_IsStallEP (USBD_HandleTypeDef *pdev, uint8_t ep_addr) {
	PCD_HandleTypeDef *hpcd = pdev->pData;
	if (ep_addr & 0x80) {
		return hpcd->IN_ep[ep_addr & 0x7F].is_stall;
	} else {
		return hpcd->OUT_ep[ep_addr & 0x7F].is_stall;
	}
}

USBD_StatusTypeDef USBD_LL_SetUSBAddress(USBD_HandleTypeDef *pdev, uint8_t dev_addr) {
	HAL_PCD_SetAddress((PCD_HandleTypeDef*)pdev->pData, dev_addr);
	return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_Transmit(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size) {
	HAL_PCD_EP_Transmit((PCD_HandleTypeDef*)pdev->pData, ep_addr, pbuf, size);
	return USBD_OK;
}

USBD_StatusTypeDef USBD_LL_PrepareReceive(USBD_HandleTypeDef *pdev, uint8_t ep_addr, uint8_t *pbuf, uint16_t size) {
	HAL_PCD_EP_Receive((PCD_HandleTypeDef*)pdev->pData, ep_addr, pbuf, size);
	return USBD_OK;
}

uint32_t USBD_LL_GetRxDataSize  (USBD_HandleTypeDef *pdev, uint8_t  ep_addr) {
	return HAL_PCD_EP_GetRxCount((PCD_HandleTypeDef*)pdev->pData, ep_addr);
}

void  USBD_LL_Delay (uint32_t Delay) {
	HAL_Delay(Delay);
}

/* wrappers */
void HAL_PCD_DataOutStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_DataOutStage(pDev, epnum, hpcd->OUT_ep[epnum].xfer_buff);
}

void HAL_PCD_DataInStageCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_DataInStage(pDev, epnum, hpcd->IN_ep[epnum].xfer_buff);
}

void HAL_PCD_SetupStageCallback(PCD_HandleTypeDef *hpcd) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_SetupStage(pDev, (uint8_t *)hpcd->Setup);
}

void HAL_PCD_SOFCallback(PCD_HandleTypeDef *hpcd) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_SOF(pDev);
}

void HAL_PCD_ResetCallback(PCD_HandleTypeDef *hpcd) {
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);

	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
	USBD_SpeedTypeDef speed = USBD_SPEED_FULL;
	switch (hpcd->Init.speed) {
		case PCD_SPEED_HIGH:
			speed = USBD_SPEED_HIGH;
			break;
		case PCD_SPEED_FULL:
			speed = USBD_SPEED_FULL;
			break;
		default:
			speed = USBD_SPEED_FULL;
			break;
	}

	USBD_LL_SetSpeed(pDev, speed);
	USBD_LL_Reset(pDev);
}

void HAL_PCD_SuspendCallback(PCD_HandleTypeDef *hpcd) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_Suspend(pDev);
}

void HAL_PCD_ResumeCallback(PCD_HandleTypeDef *hpcd) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_Resume(pDev);
}

void HAL_PCD_ISOOUTIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_IsoOUTIncomplete(pDev, epnum);
}

void HAL_PCD_ISOINIncompleteCallback(PCD_HandleTypeDef *hpcd, uint8_t epnum) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_IsoINIncomplete(pDev, epnum);
}

void HAL_PCD_ConnectCallback(PCD_HandleTypeDef *hpcd) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_DevConnected(pDev);
}

void HAL_PCD_DisconnectCallback(PCD_HandleTypeDef *hpcd) {
	USBD_HandleTypeDef *pDev = (USBD_HandleTypeDef*)hpcd->pData;
trace_printf("USB [Callback] %s \n\r", __FUNCTION__);
	USBD_LL_DevDisconnected(pDev);
}

static void initUSB_NVIC(void) {
	HAL_NVIC_EnableIRQ(OTG_FS_WKUP_IRQn);
	HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

	HAL_NVIC_EnableIRQ(OTG_HS_EP1_OUT_IRQn);
	HAL_NVIC_EnableIRQ(OTG_HS_EP1_IN_IRQn);
	HAL_NVIC_EnableIRQ(OTG_HS_WKUP_IRQn);
	HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
}
