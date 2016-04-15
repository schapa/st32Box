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

USBD_HandleTypeDef USBD_Device;

void USB_GenericInit(void) {
	extern USBD_DescriptorsTypeDef VCP_Desc;
	extern USBD_ClassTypeDef USBD_CDC;
	extern USBD_CDC_ItfTypeDef SHPA_CDC_FOPS;
	USBD_Init(&USBD_Device, &VCP_Desc, 0x32);
	USBD_RegisterClass(&USBD_Device, &USBD_CDC);
	USBD_CDC_RegisterInterface(&USBD_Device, &SHPA_CDC_FOPS);
	USBD_Start(&USBD_Device);
}

/* interrupts */
void OTG_FS_WKUP_IRQHandler(void) {
	HAL_PCD_IRQHandler((PCD_HandleTypeDef*)USBD_Device.pData);
}

void OTG_FS_IRQHandler(void) {
	HAL_PCD_IRQHandler((PCD_HandleTypeDef*)USBD_Device.pData);
}


void OTG_HS_EP1_OUT_IRQHandler(void) {
	HAL_PCD_IRQHandler((PCD_HandleTypeDef*)USBD_Device.pData);
}

void OTG_HS_EP1_IN_IRQHandler(void) {
	HAL_PCD_IRQHandler((PCD_HandleTypeDef*)USBD_Device.pData);
}

void OTG_HS_WKUP_IRQHandler(void) {
	HAL_PCD_IRQHandler((PCD_HandleTypeDef*)USBD_Device.pData);
}

void OTG_HS_IRQHandler(void) {
	HAL_PCD_IRQHandler((PCD_HandleTypeDef*)USBD_Device.pData);
}
