/*
 * USB_Generic.h
 *
 *  Created on: Apr 21, 2016
 *      Author: shapa
 */

#ifndef USB_GENERIC_H_
#define USB_GENERIC_H_

#include "usbd_dev_dsc.h"
#include <stdint.h>

void USB_ACM_devInit(void);
size_t USB_ACM_write(uint8_t *pBuff, size_t size);


#endif /* USB_GENERIC_H_ */
