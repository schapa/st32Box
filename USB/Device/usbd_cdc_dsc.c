/*
 * usbd_cdc_dsc.c
 *
 *  Created on: Apr 13, 2016
 *      Author: shapa
 */


#include "usbd_core.h"
#include "usbd_conf.h"
#include "usbd_dev_dsc.h"

#define USBD_VCP_VID                      0x0483
#define USBD_VCP_PID                      0x5740
#define USBD_VCP_LANGID_STRING            0x409
#define USBD_VCP_MANUFACTURER_STRING      "Simple manufacture"
#define USBD_VCP_PRODUCT_HS_STRING        "Just a test Product. HS mode"
#define USBD_VCP_SERIALNUMBER_HS_STRING   "JakieHS"
#define USBD_VCP_PRODUCT_FS_STRING        "Just a test Product. FS mode"
#define USBD_VCP_SERIALNUMBER_FS_STRING   "JakieFS"
#define USBD_VCP_CONFIGURATION_HS_STRING  "VCP Config"
#define USBD_VCP_INTERFACE_HS_STRING      "VCP Interface"
#define USBD_VCP_CONFIGURATION_FS_STRING  USBD_VCP_CONFIGURATION_HS_STRING
#define USBD_VCP_INTERFACE_FS_STRING      USBD_VCP_INTERFACE_HS_STRING

static uint8_t* deviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t* langIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t* manufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t* productStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t* serialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t* configStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);
static uint8_t* interfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length);

USBD_DescriptorsTypeDef USBD_CDC_ACM_DSC = {
	deviceDescriptor,
	langIDStrDescriptor,
	manufacturerStrDescriptor,
	productStrDescriptor,
	serialStrDescriptor,
	configStrDescriptor,
	interfaceStrDescriptor,
};

static uint8_t USBD_VCP_DeviceDesc[USB_LEN_DEV_DESC] = {
	0x12,                       /* bLength */
	USB_DESC_TYPE_DEVICE,       /* bDescriptorType */
	0x00,                       /* bcdUSB */
	0x02,
	0x00,                       /* bDeviceClass */
	0x00,                       /* bDeviceSubClass */
	0x00,                       /* bDeviceProtocol */
	USB_MAX_EP0_SIZE,           /* bMaxPacketSize */
	LOBYTE(USBD_VCP_VID),           /* idVendor */
	HIBYTE(USBD_VCP_VID),           /* idVendor */
	LOBYTE(USBD_VCP_PID),           /* idVendor */
	HIBYTE(USBD_VCP_PID),           /* idVendor */
	0x00,                       /* bcdDevice rel. 2.00 */
	0x02,
	USBD_IDX_MFC_STR,           /* Index of manufacturer string */
	USBD_IDX_PRODUCT_STR,       /* Index of product string */
	USBD_IDX_SERIAL_STR,        /* Index of serial number string */
	USBD_MAX_NUM_CONFIGURATION  /* bNumConfigurations */
}; /* USB_DeviceDescriptor */

static uint8_t* s_langIDDesc[USB_LEN_LANGID_STR_DESC] = {
	(uint8_t*)USB_LEN_LANGID_STR_DESC,
	(uint8_t*)USB_DESC_TYPE_STRING,
	(uint8_t*)LOBYTE(USBD_VCP_LANGID_STRING),
	(uint8_t*)HIBYTE(USBD_VCP_LANGID_STRING),
};

static uint8_t s_strDesc[USBD_MAX_STR_DESC_SIZ];

static uint8_t* deviceDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	(void)speed;
	*length = USBD_VCP_DeviceDesc[0];
	return USBD_VCP_DeviceDesc;
}

static uint8_t* langIDStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	(void)speed;
	*length = USB_LEN_LANGID_STR_DESC;
	return (uint8_t*)s_langIDDesc;
}

static uint8_t* productStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if(speed == 0) {
		USBD_GetString((uint8_t *)USBD_VCP_PRODUCT_HS_STRING, s_strDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_VCP_PRODUCT_FS_STRING, s_strDesc, length);
	}
	return s_strDesc;
}

static uint8_t* manufacturerStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	(void)speed;
	USBD_GetString((uint8_t *)USBD_VCP_MANUFACTURER_STRING, s_strDesc, length);
	return s_strDesc;
}

static uint8_t* serialStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if(speed == USBD_SPEED_HIGH) {
		USBD_GetString((uint8_t *)USBD_VCP_SERIALNUMBER_HS_STRING, s_strDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_VCP_SERIALNUMBER_FS_STRING, s_strDesc, length);
	}
	return s_strDesc;
}

static uint8_t* configStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if(speed == USBD_SPEED_HIGH) {
		USBD_GetString((uint8_t *)USBD_VCP_CONFIGURATION_HS_STRING, s_strDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_VCP_CONFIGURATION_FS_STRING, s_strDesc, length);
	}
	return s_strDesc;
}

static uint8_t* interfaceStrDescriptor(USBD_SpeedTypeDef speed, uint16_t *length) {
	if(speed == 0) {
		USBD_GetString((uint8_t *)USBD_VCP_INTERFACE_HS_STRING, s_strDesc, length);
	} else {
		USBD_GetString((uint8_t *)USBD_VCP_INTERFACE_FS_STRING, s_strDesc, length);
	}
	return s_strDesc;
}
