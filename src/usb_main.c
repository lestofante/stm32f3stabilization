/*

 * usb_main.h
 *
 *  Created on: 04/mag/2013
 *      Author: mauro
 */

//#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"

__IO uint8_t PrevXferComplete = 1;
__IO uint8_t READ_DONE = 1;

//__IO uint8_t contatoreBufferUSB;

#define BUFFER 180

uint8_t b1_tempBuffer[BUFFER];
uint8_t b2_tempBuffer[BUFFER];

uint8_t *scrivimi;
uint8_t *elaborazione;
volatile uint8_t contatoreBufferUSB = 0;
uint8_t byteStuffing = 7;
volatile uint8_t count_byte_stuffing = 0;
uint8_t stuffing_inizio = 'i';
uint8_t stuffing_fine = 'f';
uint8_t stuffing_break = 'b';

void scriviBuffer(){
	if (READ_DONE != 0){
		uint16_t addr;
		if (PrevXferComplete){
			addr = 0x098;
		}else{
			addr = 0x14c;
		}
		uint8_t *tmpBuffer;
		tmpBuffer = scrivimi;
		scrivimi = elaborazione;
		elaborazione = tmpBuffer;

		uint8_t i;

		UserToPMABufferCopy(elaborazione, addr, BUFFER);

		for(i = byteStuffing; i<BUFFER_SIZE; i++){
			scrivimi[i] = stuffing_break; //EMPTY MESSAGE
		}

		contatoreBufferUSB = 0;

		count_byte_stuffing=0;

		READ_DONE = 0;
		SetEPTxValid(ENDP1);
	}
}

void USB_flush(){
	while(READ_DONE == 0)
		;
	scriviBuffer();
}

int USB_writeByte(uint8_t dato){
	scriviBuffer(); //write if we can

	if (contatoreBufferUSB >= BUFFER-byteStuffing-1) //check if we have free space in buffer, -3 because of our "end of data" is 0xFFFFFF
		return 0; //sorry, no space left on buffer.


	scrivimi[contatoreBufferUSB++] = dato; //write the byte

	return 1; //we wrote 1 byte! wow
}

int USB_writeByteBlocking(uint8_t dato){
	while (USB_writeByte(dato)< 1)
		scriviBuffer();
	return 1;
}

/*
void USB_update(){
	if(PrevXferComplete){
		UserToPMABufferCopy(Buffer, 0x098, 180);
		updateBuffer(fast);
	}else{
		UserToPMABufferCopy(Buffer2, 0x14c, 180);
		updateBuffer2(fast);
	}
}

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
*/

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

	scrivimi = b1_tempBuffer;
	elaborazione = b2_tempBuffer;

	while (bDeviceState != CONFIGURED)
	{}
}
