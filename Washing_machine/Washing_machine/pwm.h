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

#define MOTOR_SPEED_MAX	250			// 모터 스피드 최대값
#define MOTOR_SPEED_MIN	30			// 모터 스피드 최소값 (0은 STOP상태)
#define MOTOR_STOP		0
#define MOTOR_START		250			// 모터 속도 최초값 

#define MOTOR_PWM OCR3C

#define MOTOR_DIR_DDR	DDRF
#define MOTOR_DIR_PORT PORTF

#define IN1		6
#define IN2		7

void motor_stop(void);
void change_dir(int n);
void motor_dir_toggle(void);
int motor_get_dir(void);

void motor_speed_up_num(int n);
void motor_speed_up_20(void);
void motor_speed_down_num(int n);
void motor_speed_down_20(void);

void motor_set_speed_num(int n);
void motor_set_speed_max(void);
void motor_set_speed_min(void);

void init_timer3(void);
void pwm_fan_control_main(void);
void init_L298N(void);

#endif /* PWM_H_ */