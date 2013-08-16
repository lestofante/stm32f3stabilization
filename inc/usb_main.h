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

 void USB_Config(void);
 void USB_write(uint8_t*,uint16_t);

#ifdef __cplusplus
 }
#endif

#endif /* USB_MAIN_H_ */
