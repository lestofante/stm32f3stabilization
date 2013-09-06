/*
 * pwm.h
 *
 *  Created on: 06/set/2013
 *      Author: mauro
 */


#ifndef PWM_H_
#define PWM_H_

#include "stm32f30x.h"
#include "stm32f3_discovery.h"
#include "stm32f30x_conf.h"

//BECAUSE 800
#define PWM_MIN 8
#define PWM_MAX 18

extern void init_pwm_tim4(void);
extern void setPwm(uint8_t, uint8_t, uint8_t, uint8_t);

#endif /* PWM_H_ */
