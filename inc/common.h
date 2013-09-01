/*
 * common.h
 *
 *  Created on: Nov 14, 2012
 *      Author: andrei
 */

#ifndef COMMON_H_
#define COMMON_H_

#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f30x_conf.h"
#include <stdio.h>
#include <stdint.h>

extern uint32_t micros();
extern void DelayMs(uint32_t);
extern void DelayUs(uint32_t);

#endif /* COMMON_H_ */
