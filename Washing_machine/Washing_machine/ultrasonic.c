/*
 * ultrasonic.c
 *
 * Created: 2025-03-12 오후 2:49:18
 *  Author: microsoft
 */ 

#include "ultrasonic.h"

extern volatile int ultrasonic_check_timer;


volatile int pre_tcnt3 = 0;
volatile int ultrasonic_dis = 0;
//volatile char scm[50];

// 상승, 하강 엣지 둘 다 에서 들어옴!
ISR(INT4_vect)
{
	// 1. rising edge -> 지금 echo는 HIGH
	if(ECHO_PIN & 1 << ECHO)
	{
		TCNT3 = 0; // 16bit counter 개수 세기 시작!!
					// 여기서 clear하니까 init에서 굳이 안했구낭
					// TCNT1 레지스터의 값 자체는 인터럽트랑 관련없이 그냥 timer에 맞춰서 증가하니까!
	}
	// 2. falling edge -> 지금 echo는 LOW
	else
	{
		// ECHO핀에 들어온 펄스 개수를 us로 환산한다
		//ultrasonic_dis = 1000000.0 * TCNT3 * 4;
		ultrasonic_dis = 1000000.0 * TCNT1 * 64 / F_CPU;
		ultrasonic_dis /= 58;
		
		
		
		// ex) TCNT1 == 10 일 때
		// 15.625KHz의 1주기는 64us
		// 즉,,, 지금은 640us가 지난시점 == 0.00064sec가 지남!!!
		// 초음파가 1cm에 58us가 걸리니까,,,
		// 640us / 58us ==> 11cm
		//sprintf(scm, "dis : %dcm\n",ultrasonic_dis / 58); // 화면에 출력하지 말고 scm에 넣기
	}

}


void init_ultrasonic(void)
{
	// ddr
	TRIG_DDR |= 1 << TRIG;		// output mode
	ECHO_DDR &= ~(1 << ECHO);	// input mode로 설정 
	//ECHO_DDR &= ~(1 << TRIG); // 왜 TRIG로 하셨지,,,,,
	
	// interrupt 설정 (echo, timer)
	
	// rising edge, falling edge 둘 다 인터럽트 받음 == ISCn1, ISCn0 를 01로 설정
	EICRB |= 0 << ISC41 | 1 << ISC40; // 여기도 EICRB가 처음에는 0으로 되어있다고 가정하는걸까,,
	
	// timer 설정 (16비트 타이머 1번 사용) <- 시간측정이 필요하니까!!
	// -> timer paulse가 몇개인지 확인하면, ECHO HIGH 길이 (시간)을 알 수 있음!!
	
	// 16MHz를 1024로 분주 16000000Hz / 1024 = 15625Hz == 15.625KHz
	// 1주기 T = 1 / 15625 = 0.000064s = 0.064ms = 64us
	
	// 1024로 분주
	//TCCR1B  |= 1 << CS12 | 0 << CS11 | 1 << CS10;
	
	// 8분주 -> 16000000 / 8 = 2000000Hz = 2MHz
	// 1주기에 걸리는 시간 = 1/2000000 = 0.0000005sec = 0.5us
	
	// local interrupt 마스크
	EIMSK |= 1 << INT4; // EXTERNAL interrupt 활성화
	// 타이머는 안하남,,,,,,,,
	// 그냥 타이머에 맞춰 자연증가하는 TCNT값을 사용하는거지 overflow를 사용하는게 아니라 인터럽트 활성화할필요가없나봄!
	// 그냥 카운터로만 쓰는것!
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
	
	// 10us동안 보내기 -> 약간 여유있게 15로 했당
	_delay_us(15);
	
	// 다시 LOW로 돌리기
	TRIG_PORT &= ~(1 << TRIG);
}

void distance_ultrasonic(void)
{
	// 1초체크
	if(ultrasonic_check_timer >= 1000)
	{
		ultrasonic_check_timer = 0;
		//printf("%s", scm); // 저장해둔 거리text 출력
		
		trigger_ultrasonic(); // 출력하고 트리거 또 보내기!
	}
}