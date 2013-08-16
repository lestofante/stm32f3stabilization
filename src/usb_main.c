/*

 * usb_main.h
 *
 *  Created on: 04/mag/2013
 *      Author: mauro
 */

#include "usb_lib.h"
#include "usb_pwr.h"

__IO uint8_t NEXT_BUFFER_W = 0;
__IO uint8_t NEXT_BUFFER_R = 0;
__IO uint8_t DATA_PRESENT[2];
__IO uint8_t DATA[2];

void USB_write(uint8_t* toWrite, uint16_t size){
	if(size>63) size = 63;
	if(size<0) size = 0;

	while(DATA_PRESENT[NEXT_BUFFER_W]);
	uint16_t addr;

	if(!NEXT_BUFFER_W){
		addr = ENDP1_BUF0;
	}else{
		addr = ENDP1_BUF1;
	}

	toWrite[0] = addr>>8;
	toWrite[1] = addr;

	UserToPMABufferCopy(toWrite, addr, size);
	DATA[NEXT_BUFFER_W] = size;

	DATA_PRESENT[NEXT_BUFFER_W] = 1;
	NEXT_BUFFER_W = !NEXT_BUFFER_W;

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

	DATA_PRESENT[0] = 0;
	DATA_PRESENT[1] = 0;

	USB_Init();

	while (bDeviceState != CONFIGURED)
	{}
}
