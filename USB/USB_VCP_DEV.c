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
trace_printf(" cdm %d, len %d \n\r", cmd, len);
//#define CDC_SEND_ENCAPSULATED_COMMAND               0x00
//#define CDC_GET_ENCAPSULATED_RESPONSE               0x01
//#define CDC_SET_COMM_FEATURE                        0x02
//#define CDC_GET_COMM_FEATURE                        0x03
//#define CDC_CLEAR_COMM_FEATURE                      0x04
//#define CDC_SET_LINE_CODING                         0x20
//#define CDC_GET_LINE_CODING                         0x21
//#define CDC_SET_CONTROL_LINE_STATE                  0x22
//#define CDC_SEND_BREAK                              0x23
	return USBD_OK;
}

static int8_t Receive(uint8_t *buff, uint32_t *len) {
trace_printf("USB [VCP] %s", __FUNCTION__);
trace_printf("len %d \n\r", len);
	return USBD_OK;
}
