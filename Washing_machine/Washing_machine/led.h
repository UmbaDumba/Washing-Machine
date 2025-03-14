/*
 * led.h
 *
 * Created: 2025-03-05 오전 10:21:21
 *  Author: microsoft
 */ 

#ifndef LED_H_
#define LED_H_

#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us

#include "def.h"

#define DELAY 50

#define LED_DDR DDRB
#define LED_PORT PORTB
#define LED1 7
#define LED2 6
#define LED3 4
#define LED4 3
#define LED5 2
#define LED6 0
#define LED_COUNT 6
#define LED_FUNCTION 6

int led_main(void);   // 선언
void led_init(void);
void led_all_on(void);
void led_all_off(void);
void shift_left_ledon(void);
void shift_right_ledon(void);
void shift_left_keep_ledon(void);
void shift_right_keep_ledon(void);
void flower_on(void);
void flower_off(void);

void make_pwm_led_control(void);
void led_on_pwm(int dim);
void led_on_range(int led_count);

#endif /* LED_H_ */