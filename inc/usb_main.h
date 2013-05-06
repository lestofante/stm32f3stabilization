/*
 * usb_main.h
 *
 *  Created on: 04/mag/2013
 *      Author: mauro
 */

#ifndef USB_MAIN_H_
#define USB_MAIN_H_

#ifdef __cplusplus
 extern "C" {
#endif

 void USB_flush(void);
 int USB_writeByte(uint8_t dato);
 int USB_writeByteBlocking(uint8_t dato);
 void USB_Config(void);

#ifdef __cplusplus
 }
#endif

#endif /* USB_MAIN_H_ */
