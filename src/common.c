/*
 * common.c
 *
 *  Created on: Nov 14, 2012
 *      Author: andrei
 */


#include "common.h"


volatile uint32_t uSec = 0;

volatile uint32_t pwmEnd[] = {100000, 200000, 300000, 400000};
/**
 * @brief  Get time from power up
 * @param  none
 * @retval microseconds from code start
 */
uint32_t micros()
{
	return uSec;
}

void DelayMs(uint32_t delay){
	uint32_t i;
	for (i=0; i < delay; i++){ //be sure we don't overflow!!!
		DelayUs(1000);
	}
}

void DelayUs(uint32_t delay){
	uint32_t init = micros();
	while (micros()-init < delay){//anti-overflow code!
		;//just wait
	}
}


/**********************************/
void SysTick_Handler(void)
{
	uSec++;

	if (uSec%1000000UL == 0){//every Sec, heartbeat
		/* Toggle LD5 */
		STM_EVAL_LEDToggle(LED3);
	}
}
