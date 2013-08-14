/*

 * usb_main.h
 *
 *  Created on: 04/mag/2013
 *      Author: mauro
 */

//#include "hw_config.h"
#include "usb_lib.h"
#include "usb_pwr.h"

__IO bool NEXT_BUFFER_W = 0;
__IO bool NEXT_BUFFER_R = 0;
__IO bool* DATA_PRESENT;

//__IO uint8_t contatoreBufferUSB;

#define BUFFER 64

uint8_t b1_tempBuffer[BUFFER];

uint8_t *scrivimi;
volatile uint8_t contatoreBufferUSB = 0;
uint8_t byteStuffing = 9;
uint8_t stuffing_inizio = 'i';
uint8_t stuffing_fine = 'f';
uint8_t stuffing_break = 'b';


void scriviBuffer(){
	if(!DATA_PRESENT[NEXT_BUFFER_W]){
		uint16_t addr;

		if (NEXT_BUFFER_W){
			addr = 0x098;
		}else{
			addr = 0x0D8;
		}

		uint8_t i;

		scrivimi[1] = addr;
		UserToPMABufferCopy(scrivimi, addr, BUFFER);

		scrivimi[0]++; //packet sequence number

		for(i = 1; i<byteStuffing; i++){
			scrivimi[i] = stuffing_inizio; //EMPTY MESSAGE
		}
		for(i = byteStuffing+1; i<BUFFER_SIZE; i++){
			scrivimi[i] = stuffing_break; //EMPTY MESSAGE
		}

		contatoreBufferUSB = byteStuffing;

		DATA_PRESENT[NEXT_BUFFER_W] = 1;
		NEXT_BUFFER_W = !NEXT_BUFFER_W;

		SetEPTxCount(ENDP1,BUFFER);
		SetEPTxValid(ENDP1);
	}
}

void USB_flush(){
	scriviBuffer();
}

int USB_writeByte(uint8_t dato){
	scriviBuffer(); //write if we can

	if (contatoreBufferUSB >= BUFFER-byteStuffing-1) //check if we have free space in buffer, -byteStuffing because of our "end of data"
		return 0; //sorry, no space left on buffer.


	scrivimi[contatoreBufferUSB++] = dato; //write the byte

	return 1; //we wrote 1 byte! wow
}

int USB_writeByteBlocking(uint8_t dato){
	while (USB_writeByte(dato)< 1)
		scriviBuffer();
	return 1;
}

int USB_writeAtomic(uint8_t* dati, uint16_t size){
	if (contatoreBufferUSB+size >= BUFFER-byteStuffing-1) //check if we have free space in buffer, -byteStuffing because of our "end of data"
		return 0; //sorry, no space left on buffer.

	uint16_t i=0;
	for (i=0;i<size; i++){
		scrivimi[contatoreBufferUSB++] = dati[i]; //write the byte
	}

	return size; //we wrote 1 byte! wow
}

int USB_writeAtomicBlocking(uint8_t* dati, uint16_t size){
	scriviBuffer();
	while (USB_writeAtomic(dati, size)!= size)
		scriviBuffer();
	return size;
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
	int i;

	Set_System();
	Set_USBClock();
	USB_Interrupts_Config();

	USB_Init();

	scrivimi = b1_tempBuffer;

	for(i = 0; i<byteStuffing; i++){
		scrivimi[i] = stuffing_inizio; //EMPTY MESSAGE
	}

	for(i = byteStuffing; i<BUFFER_SIZE; i++){
		scrivimi[i] = stuffing_break; //EMPTY MESSAGE
	}

	while (bDeviceState != CONFIGURED)
	{}
}
