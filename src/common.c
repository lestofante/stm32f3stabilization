/*
 * common.c
 *
 *  Created on: Nov 14, 2012
 *      Author: andrei
 */


#include "common.h"


__IO uint32_t uSec = 0;


/**
 * @brief  Get time from power up
 * @param  none
 * @retval microseconds from code start
 */
uint32_t micros()
{
	return uSec;
}


/**********************************/
void SysTick_Handler(void)
{
	uSec++;
}

