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

#include "USB_Generic.h"

static USBD_HandleTypeDef USBD_Device;

void USB_ACM_devInit(void) {
	extern USBD_CDC_ItfTypeDef SHPA_CDC_FOPS;

	USBD_Init(&USBD_Device, USBD_ACM_DSC, 0x32);
	USBD_RegisterClass(&USBD_Device, USBD_CDC_CLASS);
	USBD_CDC_RegisterInterface(&USBD_Device, &SHPA_CDC_FOPS);

	USBD_Start(&USBD_Device);
}

size_t USB_ACM_write(uint8_t *pBuff, size_t size) {
	size_t written = 0;
	void *cpy = NULL;
	__disable_irq();
	cpy = malloc(size);
	__enable_irq();
	if (cpy) {
		memcpy(cpy, pBuff, size);
	    USBD_CDC_SetTxBuffer(&USBD_Device, cpy, size);
	    USBD_CDC_TransmitPacket(&USBD_Device);
	}
	return written;
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
