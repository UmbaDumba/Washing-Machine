﻿/*
 * ultrasonic.h
 *
 * Created: 2025-03-12 오후 2:49:06
 *  Author: microsoft
 */ 


#ifndef ULTRASONIC_H_
#define ULTRASONIC_H_

#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us
#include <avr/interrupt.h>

#define TRIG 4  // echo pin 번호
#define TRIG_PORT PORTG
#define TRIG_DDR DDRG

#define ECHO 4 // echo pin 번호
#define ECHO_PIN PINE // External INT 4
#define ECHO_DDR DDRE

void init_ultrasonic(void);
void trigger_ultrasonic(void);
void distance_ultrasonic(void);

#endif /* ULTRASONIC_H_ */