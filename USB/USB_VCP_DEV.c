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


static int8_t Init(void);
static int8_t DeInit(void);
static int8_t Control(uint8_t cmd, uint8_t *buff, uint16_t len);
static int8_t Receive(uint8_t *buff, uint32_t *len);

USBD_CDC_ItfTypeDef SHPA_CDC_FOPS = {
		Init,
		DeInit,
		Control,
		Receive
};

static USBD_CDC_LineCodingTypeDef s_vcpLinecoding = {
	115200, /* baud rate*/
	0x00,   /* stop bits-1*/
	0x00,   /* parity - none*/
	0x08    /* nb. of bits 8*/
};

static int8_t Init(void) {
trace_printf("USB [VCP] %s \n\r", __FUNCTION__);

	return USBD_OK;
}

static int8_t DeInit(void) {
trace_printf("USB [VCP] %s \n\r", __FUNCTION__);
	return USBD_OK;
}

static int8_t Control(uint8_t cmd, uint8_t *buff, uint16_t len) {
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
			s_vcpLinecoding.bitrate    = (uint32_t)((buff[0] | (buff[1] << 8) | (buff[2] << 16) | (buff[3] << 24)));
			s_vcpLinecoding.format     = buff[4];
			s_vcpLinecoding.paritytype = buff[5];
			s_vcpLinecoding.datatype   = buff[6];
			trace_printf("\t bitrate %d \n\r", s_vcpLinecoding.bitrate);
			trace_printf("\t format %d \n\r", s_vcpLinecoding.format);
			trace_printf("\t paritytype %d \n\r", s_vcpLinecoding.paritytype);
			trace_printf("\t datatype %d \n\r", s_vcpLinecoding.datatype);
			break;
		case CDC_GET_LINE_CODING:
			trace_printf("CDC_GET_LINE_CODING len %d \n\r", len);
			buff[0] = (uint8_t)(s_vcpLinecoding.bitrate);
			buff[1] = (uint8_t)(s_vcpLinecoding.bitrate >> 8);
			buff[2] = (uint8_t)(s_vcpLinecoding.bitrate >> 16);
			buff[3] = (uint8_t)(s_vcpLinecoding.bitrate >> 24);
			buff[4] = s_vcpLinecoding.format;
			buff[5] = s_vcpLinecoding.paritytype;
			buff[6] = s_vcpLinecoding.datatype;
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

static int8_t Receive(uint8_t *buff, uint32_t *len) {
trace_printf("USB [VCP] %s %d\n\r", __FUNCTION__, *len);
	uint32_t i = 0;
	for (i = 0; i < *len; i++) {
		trace_printf(" %d", buff[i]);
	}
	trace_printf("\n\r");

	return USBD_OK;
}
