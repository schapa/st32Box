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
#include "dbg_base.h"

#include <stddef.h>
#include <stdbool.h>
#include <stdlib.h>

#if 01
#include "dbg_trace.h"
#endif

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
		{ 0 },
		"Hello My little master"
};


static int8_t Init(USBD_HandleTypeDef *pDevice) {
	DBG_ENTRY;
	USBD_CDC_ItfTypeDef *pDevControl = (USBD_CDC_ItfTypeDef*)pDevice->pUserData;
	DBGMSG_M("handle %p", pDevControl);
	pDevControl->property = s_vcpDefaultSettings;
	USBD_CDC_SetTxBuffer(pDevice, s_txBuffer, 0);
	USBD_CDC_SetRxBuffer(pDevice, s_rxBuffer);

	DBG_EXIT;
	return USBD_OK;
}

static int8_t DeInit(USBD_HandleTypeDef *pDevice) {
	DBG_ENTRY;
	(void)pDevice;
	DBGMSG_M("handle %p", pDevice);
	DBG_EXIT;
	return USBD_OK;
}

static int8_t Control(USBD_HandleTypeDef *pDevice, uint8_t cmd, uint8_t *buff, uint16_t len) {
	DBG_ENTRY;
	USBD_CDC_ItfTypeDef *pDevControl = (USBD_CDC_ItfTypeDef*)pDevice->pUserData;
	switch (cmd) {
		case CDC_SEND_ENCAPSULATED_COMMAND:
			DBGMSG_M("ENCAPSULATED_COMMAND len %d", len);
			break;
		case CDC_GET_ENCAPSULATED_RESPONSE:
			DBGMSG_M("ENCAPSULATED_RESPONSE len %d", len);
			break;
		case CDC_SET_COMM_FEATURE:
			DBGMSG_M("SET_COMM_FEATURE len %d", len);
			break;
		case CDC_GET_COMM_FEATURE:
			DBGMSG_M("GET_COMM_FEATURE len %d", len);
			break;
		case CDC_CLEAR_COMM_FEATURE:
			DBGMSG_M("CLEAR_COMM_FEATURE len %d", len);
			break;
		case CDC_SET_LINE_CODING:
			DBGMSG_M("SET_LINE_CODING len %d", len);
			pDevControl->property.bitrate    = (uint32_t)((buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24)));
			pDevControl->property.format     = buff[4];
			pDevControl->property.paritytype = buff[5];
			pDevControl->property.datatype   = buff[6];
			DBGMSG_M("bitrate %d", pDevControl->property.bitrate);
			DBGMSG_M("format %d", pDevControl->property.format);
			DBGMSG_M("paritytype %d", pDevControl->property.paritytype);
			DBGMSG_M("datatype %d", pDevControl->property.datatype);
			break;
		case CDC_GET_LINE_CODING:
			DBGMSG_M("CDC_GET_LINE_CODING len %d", len);
			buff[0] = (uint8_t)(pDevControl->property.bitrate);
			buff[1] = (uint8_t)(pDevControl->property.bitrate >> 8);
			buff[2] = (uint8_t)(pDevControl->property.bitrate >> 16);
			buff[3] = (uint8_t)(pDevControl->property.bitrate >> 24);
			buff[4] = pDevControl->property.format;
			buff[5] = pDevControl->property.paritytype;
			buff[6] = pDevControl->property.datatype;
			break;
		case CDC_SET_CONTROL_LINE_STATE:
			DBGMSG_M("SET_CONTROL_LINE_STATE len %d", len);
			break;
		case CDC_SEND_BREAK:
			DBGMSG_M("SEND_BREAK len %d", len);
			break;
		default:
			DBGMSG_WARN("cdm %d, len %d", cmd, len);
			break;
	}
	DBG_EXIT;
	return USBD_OK;
}

static int8_t Receive(USBD_HandleTypeDef *pDevice, uint8_t *buff, uint32_t *len) {
	DBG_ENTRY;
	(void)pDevice;

	DBGMSG_M("Count %d", *len);
	buff[*len] = '\0';
	DBGMSG_M("[%s]", buff);

	/* echo */
    USBD_CDC_SetTxBuffer(pDevice, buff, *len);
    USBD_CDC_TransmitPacket(pDevice);

	USBD_CDC_ReceivePacket(pDevice);

	DBG_EXIT;
	return USBD_OK;
}
