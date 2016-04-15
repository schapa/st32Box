/*
 * USB_VCP_DEV.c
 *
 *  Created on: Apr 13, 2016
 *      Author: shapa
 */

#include "stm32f4xx_hal.h"
#include "diag/Trace.h"
#include "usbd_core.h"
#include "usbd_cdc.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>


#define RX_DATA_SIZE  2048
#define TX_DATA_SIZE  2048

static uint8_t s_rxBuffer[RX_DATA_SIZE];
static uint8_t s_txBuffer[TX_DATA_SIZE];

static int8_t Init(USBD_HandleTypeDef *pDevice);
static int8_t DeInit(USBD_HandleTypeDef *pDevice);
static int8_t Control(USBD_HandleTypeDef *pDevice, uint8_t cmd, uint8_t *buff, uint16_t len);
static int8_t Receive(USBD_HandleTypeDef *pDevice, uint8_t *buff, uint32_t *len);


static const USBD_CDC_LineCodingTypeDef s_vcpDefaultSettings = {
	115200, /* baud rate*/
	0x00,   /* stop bits-1*/
	0x00,   /* parity - none*/
	0x08    /* nb. of bits 8*/
};

USBD_CDC_ItfTypeDef SHPA_CDC_FOPS = {
		Init,
		DeInit,
		Control,
		Receive,
		{ 0 }
};


static int8_t Init(USBD_HandleTypeDef *pDevice) {

	USBD_CDC_ItfTypeDef *pDevControl = (USBD_CDC_ItfTypeDef*)pDevice->pUserData;
	pDevControl->property = s_vcpDefaultSettings;

trace_printf("USB [VCP] %s \n\r", __FUNCTION__);

	USBD_CDC_SetTxBuffer(pDevice, s_txBuffer, 0);
	USBD_CDC_SetRxBuffer(pDevice, s_rxBuffer);
//	TODO: call it here ?
	USBD_CDC_ReceivePacket(pDevice);

	return USBD_OK;
}

static int8_t DeInit(USBD_HandleTypeDef *pDevice) {
	(void)pDevice;
trace_printf("USB [VCP] %s \n\r", __FUNCTION__);
	return USBD_OK;
}

static int8_t Control(USBD_HandleTypeDef *pDevice, uint8_t cmd, uint8_t *buff, uint16_t len) {
	USBD_CDC_ItfTypeDef *pDevControl = (USBD_CDC_ItfTypeDef*)pDevice->pUserData;
trace_printf("USB [VCP] %s ", __FUNCTION__);
	switch (cmd) {
		case CDC_SEND_ENCAPSULATED_COMMAND:
			trace_printf("ENCAPSULATED_COMMAND len %d \n\r", len);
			break;
		case CDC_GET_ENCAPSULATED_RESPONSE:
			trace_printf("ENCAPSULATED_RESPONSE len %d \n\r", len);
			break;
		case CDC_SET_COMM_FEATURE:
			trace_printf("SET_COMM_FEATURE len %d \n\r", len);
			break;
		case CDC_GET_COMM_FEATURE:
			trace_printf("GET_COMM_FEATURE len %d \n\r", len);
			break;
		case CDC_CLEAR_COMM_FEATURE:
			trace_printf("CLEAR_COMM_FEATURE len %d \n\r", len);
			break;
		case CDC_SET_LINE_CODING:
			trace_printf("SET_LINE_CODING len %d \n\r", len);
			pDevControl->property.bitrate    = (uint32_t)((buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24)));
			pDevControl->property.format     = buff[4];
			pDevControl->property.paritytype = buff[5];
			pDevControl->property.datatype   = buff[6];
			trace_printf("\t bitrate %d \n\r", pDevControl->property.bitrate);
			trace_printf("\t format %d \n\r", pDevControl->property.format);
			trace_printf("\t paritytype %d \n\r", pDevControl->property.paritytype);
			trace_printf("\t datatype %d \n\r", pDevControl->property.datatype);
			break;
		case CDC_GET_LINE_CODING:
			trace_printf("CDC_GET_LINE_CODING len %d \n\r", len);
			buff[0] = (uint8_t)(pDevControl->property.bitrate);
			buff[1] = (uint8_t)(pDevControl->property.bitrate >> 8);
			buff[2] = (uint8_t)(pDevControl->property.bitrate >> 16);
			buff[3] = (uint8_t)(pDevControl->property.bitrate >> 24);
			buff[4] = pDevControl->property.format;
			buff[5] = pDevControl->property.paritytype;
			buff[6] = pDevControl->property.datatype;
			break;
		case CDC_SET_CONTROL_LINE_STATE:
			trace_printf("SET_CONTROL_LINE_STATE len %d \n\r", len);
			break;
		case CDC_SEND_BREAK:
			trace_printf("SEND_BREAK len %d \n\r", len);
			break;
		default:
			trace_printf(" cdm %d, len %d \n\r", cmd, len);
			break;
	}
	return USBD_OK;
}

static int8_t Receive(USBD_HandleTypeDef *pDevice, uint8_t *buff, uint32_t *len) {
	(void)pDevice;
trace_printf("USB [VCP] %s %d\n\r", __FUNCTION__, *len);
	uint32_t i = 0;
	for (i = 0; i < *len; i++) {
		trace_printf(" %c", buff[i]);
	}
	trace_printf("\n\r");
	/* echo */
    USBD_CDC_SetTxBuffer(pDevice, buff, *len);
    USBD_CDC_TransmitPacket(pDevice);

	USBD_CDC_ReceivePacket(pDevice);

	return USBD_OK;
}
