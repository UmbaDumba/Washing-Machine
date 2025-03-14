/*
 * pwm.h
 *
 * Created: 2025-03-13 오후 12:48:48
 *  Author: microsoft
 */ 


#ifndef PWM_H_
#define PWM_H_

#define  F_CPU 16000000UL  // 16MHZ

#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us
#include <avr/interrupt.h>
#include <stdio.h>		// printf

#include "button.h"

void init_timer3(void);
void pwm_fan_control_main(void);
void init_L298N(void);

#endif /* PWM_H_ */