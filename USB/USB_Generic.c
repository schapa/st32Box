/*
 * USB_Generic.c
 *
 *  Created on: Apr 13, 2016
 *      Author: shapa
 */



#include "stm32f4xx_hal.h"
#include "diag/Trace.h"
#include "usbd_core.h"
#include "usbd_cdc.h"

#include <stddef.h>
#include <stdlib.h>
#include <stdbool.h>

static PCD_HandleTypeDef *s_UsbHandle = NULL;

static void initUSB_NVIC(void);
static void freeHandle(PCD_HandleTypeDef *hpcd);

//void USB_GenericInit(void) {
//	static const size_t usbStackSize = sizeof(USBD_HandleTypeDef);
//	HAL_StatusTypeDef result = HAL_ERROR;
//	PCD_InitTypeDef iface = {
//			1, // Device Endpoints number.
//			1, // Host Channels number.
//			USB_OTG_SPEED_HIGH,
//			DISABLE, // USB embedded DMA.
//			DEP0CTL_MPS_64, // Endpoint 0 Max Packet size.
//			USB_OTG_EMBEDDED_PHY,
//			DISABLE, // output of the SOF signal.
//			DISABLE, // low power mode.
//			DISABLE, // Link Power Management.
//			DISABLE, // Battery charging.
//			DISABLE, // VBUS Sensing feature.
//			DISABLE, // use of the dedicated EP1 interrupt.
//			DISABLE, // use of the external VBUS.
//	};
//
//	freeHandle(s_UsbHandle);
//
//	do {
//		s_UsbHandle = calloc(1, sizeof(PCD_HandleTypeDef));
//		if (!s_UsbHandle) {
//			trace_printf("USB [Error] Failed to allocate instance [%zu]\n\r", sizeof(PCD_HandleTypeDef));
//			break;
//		}
//		s_UsbHandle->pData = malloc(usbStackSize);
//		if (!s_UsbHandle) {
//			trace_printf("USB [Error] Failed to allocate stack [%zu]\n\r", usbStackSize);
//			break;
//		}
//
//		s_UsbHandle->Instance = USB_OTG_HS;
//		s_UsbHandle->Init = iface;
//		result = HAL_PCD_Init(s_UsbHandle);
//		if (result != HAL_OK) {
//			trace_printf("USB [Error] PHY init [%u]\n\r", result);
//			break;
//		}
//
//		result = HAL_PCD_Start(s_UsbHandle);
//		if (result != HAL_OK) {
//			trace_printf("USB [Error] Start USB [%u]\n\r", result);
//			break;
//		}
//		initUSB_NVIC();
//	} while (false);
//
//	if (result != HAL_OK) {
//		freeHandle(s_UsbHandle);
//	}
//}

static USBD_HandleTypeDef USBD_Device;

void USB_GenericInit(void) {
	extern USBD_DescriptorsTypeDef VCP_Desc;
	extern USBD_ClassTypeDef USBD_CDC;
	extern USBD_CDC_ItfTypeDef SHPA_CDC_FOPS;
	USBD_Init(&USBD_Device, &VCP_Desc, 0x32);
	USBD_CDC_RegisterInterface(&USBD_Device, &SHPA_CDC_FOPS);
	USBD_RegisterClass(&USBD_Device, &USBD_CDC);
	s_UsbHandle = (PCD_HandleTypeDef*)USBD_Device.pData;
	USBD_Start(&USBD_Device);
}

static void initUSB_NVIC(void) {
	HAL_NVIC_EnableIRQ(OTG_FS_WKUP_IRQn);
	HAL_NVIC_EnableIRQ(OTG_FS_IRQn);

	HAL_NVIC_EnableIRQ(OTG_HS_EP1_OUT_IRQn);
	HAL_NVIC_EnableIRQ(OTG_HS_EP1_IN_IRQn);
	HAL_NVIC_EnableIRQ(OTG_HS_WKUP_IRQn);
	HAL_NVIC_EnableIRQ(OTG_HS_IRQn);
}

static void freeHandle(PCD_HandleTypeDef *hpcd) {
	if (hpcd) {
		free(hpcd->pData);
		free(hpcd);
	}
}

/* interrupts */

void OTG_FS_WKUP_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_UsbHandle);
}

void OTG_FS_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_UsbHandle);
}


void OTG_HS_EP1_OUT_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_UsbHandle);
}

void OTG_HS_EP1_IN_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_UsbHandle);
}

void OTG_HS_WKUP_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_UsbHandle);
}

void OTG_HS_IRQHandler(void) {
	HAL_PCD_IRQHandler(s_UsbHandle);
}
