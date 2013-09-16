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

enum packet_type_t {
	SENSOR_GYR,
	SENSOR_ACC,
	SENSOR_MAG,
	STRING,
	DCM,
	ANGLE,
	PWM
};

void USB_Config(void);
void USB_write(uint8_t*,uint16_t, enum packet_type_t);
int USB_read(uint8_t*);

#ifdef __cplusplus
}
#endif

#endif /* USB_MAIN_H_ */
