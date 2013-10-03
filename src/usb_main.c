/*

 * usb_main.h
 *
 *  Created on: 04/mag/2013
 *      Author: mauro
 */

#include "usb_lib.h"
#include "usb_pwr.h"
#include "usb_main.h"

__IO uint8_t READY = 1;
__IO uint8_t DATA_PRESENT = 0;

uint16_t seqN = 0;
uint8_t funziona = 0;

void USB_write(uint8_t* toWrite, uint16_t size, enum packet_type_t raw_type){

	if (!funziona)
		return;

	if(size>63-4)
		size = 63-4;

	if(size<0)
		size = 0;

	while(!READY)
		;	//wait for read to occur

	uint16_t type = raw_type;

	UserToPMABufferCopy((uint8_t*)&seqN, ENDP1_TXADDR, 2);
	UserToPMABufferCopy((uint8_t*)&type, ENDP1_TXADDR+2, 2);

	UserToPMABufferCopy(toWrite, ENDP1_TXADDR+4, size);


	seqN++;

	READY = 0;

	SetEPTxCount(ENDP1, size+4);
	SetEPTxValid(ENDP1);

}

int USB_read(uint8_t* ptr){
	if(DATA_PRESENT){
		return USB_SIL_Read(EP2_OUT, ptr);
	} else {
		return 0;
	}
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
	funziona = 1;
}
