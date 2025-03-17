/*
 * ultrasonic.c
 *
 * Created: 2025-03-12 오후 2:49:18
 *  Author: microsoft
 */ 

#include "ultrasonic.h"

extern volatile int ultrasonic_check_timer;


volatile int ultrasonic_dis = 0;


// 상승, 하강 엣지 둘 다 에서 들어옴!
ISR(INT4_vect)
{
	// 1. rising edge -> 지금 echo는 HIGH
	if(ECHO_PIN & 1 << ECHO)
	{
		TCNT2 = 0; 
	}
	// 2. falling edge -> 지금 echo는 LOW
	else
	{
		ultrasonic_dis = 1000000.0 * TCNT2 * 64 / F_CPU;
		ultrasonic_dis /= 58;
		//printf("ISR : %d\n", TCNT2);
	}

}


void init_ultrasonic(void)
{
	// ddr
	TRIG_DDR |= 1 << TRIG;		// output mode
	ECHO_DDR &= ~(1 << ECHO);	// input mode로 설정
	
	// interrupt 설정 (echo)
	EICRB |= 0 << ISC41 | 1 << ISC40; 
	
	// 64로 분주
	TCCR2 |= 1 << CS02 | 0 << CS01 | 0 << CS00;
	
	// local interrupt 마스크
	EIMSK |= 1 << INT4; // EXTERNAL interrupt 활성화
	
	TCNT2 = 0;
}

void trigger_ultrasonic(void)
{
	// TRIG 보내기 전에 일정시간 LOW로 둔 다음에 보내야함!
	// HIGH 보낸 다음에도 LOW로 다시 돌려두기
	
	TRIG_PORT &= ~(1 << TRIG); // 0으로 초기화
	
	// 약간의 딜레이
	_delay_us(1);
	
	// HIGH 쏘기!!
	TRIG_PORT |= 1 << TRIG;
	
	// 10us동안 보내기 
	_delay_us(15);
	
	// 다시 LOW로 돌리기
	TRIG_PORT &= ~(1 << TRIG);
}
