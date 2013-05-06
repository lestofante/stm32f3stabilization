/*

 * usb_main.h
 *
 *  Created on: 04/mag/2013
 *      Author: mauro
 */

//#include "hw_config.h"
#include "usb_lib.h"
#include "usb_endp.h"
#include "usb_pwr.h"

__IO uint8_t contatoreBufferUSB;
__IO uint8_t scrivimi[BUFFER_SIZE];

void USB_flush(){
	while(contatoreBufferUSB >= BUFFER_SIZE)
		;
}

int USB_writeByte(uint8_t dato){

	if (contatoreBufferUSB >= BUFFER_SIZE) //check if we have free space in buffer
		return 0; //sorry, no space left on buffer.

	scrivimi[contatoreBufferUSB++] = dato; //write the byte

	return 1; //we writed 1 byte! wow
}

int USB_writeByteBlocking(uint8_t dato){
	while (USB_writeByte(dato)< 1)
		USB_flush();
	return 1;
}


/**
 * @brief  Configure the USB.
 * @param  None
 * @retval None
 */
void USB_Config(void)
{
	Set_System();
	Set_USBClock();
	USB_Interrupts_Config();

	USB_Init();

	while (bDeviceState != CONFIGURED)
	{}
}
