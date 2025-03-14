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

#define FND_DATA_PORT  PORTC
#define FND_DATA_DDR   DDRC

#define FND_DIGIT_PORT PORTB
#define FND_DIGIT_PORT2 PORTD
#define FND_DIGIT_DDR  DDRB
#define FND_DIGIT_DDR2	DDRD

#define FND_DIGIT_D1  4
#define FND_DIGIT_D2  5
#define FND_DIGIT_D3  6
#define FND_DIGIT_D4  7

#define FND_DIGIT_D5  0
#define FND_DIGIT_D6  1
#define FND_DIGIT_D7  2
#define FND_DIGIT_D8  3

void init_fnd(void);
int fnd_main(void);
void fnd_display(void);
void fnd_display_circle(void);
void clear_digit(void);
void display_stop(void);

#endif /* FND_H_ */