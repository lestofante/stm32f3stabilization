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
#define PWM_STOP 800
#define PWM_MIN 1100
#define PWM_MAX 1800

void init_pwm_tim4(void);
void init_pwm_tim2(void);

extern void setPwm(uint16_t, uint16_t, uint16_t, uint16_t);

#endif /* PWM_H_ */
