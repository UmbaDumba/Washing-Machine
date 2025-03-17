/*
 * fnd.h
 *
 * Created: 2025-03-06 오후 12:23:18
 *  Author: microsoft
 */ 


#ifndef FND_H_
#define FND_H_

#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us

#include "pwm.h"

#define FND_DATA_PORT  PORTC
#define FND_DATA_DDR   DDRC

#define FND_DIGIT_PORT	PORTA
#define FND_DIGIT_DDR	DDRA

#define FND_DIGIT_D1  7
#define FND_DIGIT_D2  6
#define FND_DIGIT_D3  5
#define FND_DIGIT_D4  4

#define FND_DIGIT_D5  3
#define FND_DIGIT_D6  2
#define FND_DIGIT_D7  1
#define FND_DIGIT_D8  0

void init_fnd(void);
void fnd_clear_digit(void);
void fnd_display_min_sec(void);
void fnd_display_dashs(void);
void fnd_display_selectn(int mode);
void fnd_display_done(void);
void fnd_display_min_sec_num(int n);
void fnd_display_big_circle(void);

void fnd_display_washing(int secs);

#endif /* FND_H_ */