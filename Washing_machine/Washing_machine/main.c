/*
 * 01.LED_CONTROL.c
 *
 * Created: 2025-03-04 오후 4:25:34
 * Author : microsoft
 */ 

#define  F_CPU 16000000UL  // 16MHZ
#include <avr/io.h>
#include <util/delay.h>  // _delay_ms _delay_us
#include <avr/interrupt.h>
#include <stdio.h>		// printf
#include "button.h"
#include "led.h"
#include "uart0.h"
#include "pwm.h"
#include "fnd.h"

// tnd_data에 들어갈 수 있는것
// 시간정보, stop 텍스트, circle 등등

// extern----------------------------------------------------------
extern void init_ultrasonic(void);
extern void distance_ultrasonic(void);

extern int led_state;
extern void (*funcs[])(void);
extern volatile uint8_t rx_message_received;

extern uint32_t sec_count;
extern int timer_run;

// 선언 ----------------------------------------------------
void init_timer0(void);
void mode1_default(int re_wash_time);
void mode2_wool(int re_wash_time);
void mode3_blanket(int re_wash_time);
void mode3_dry(int re_wash_time);

volatile int msec_count = 0;
volatile int ultrasonic_check_timer = 0;

volatile int auto_mode = 1;
volatile int led_select = 0;

volatile int fnd_shoot = 0;

int washing_machine[] = {
	MODE_1_WASHING_TIME, 
	MODE_2_WASHING_TIME, 
	MODE_3_WASHING_TIME,
	MODE_4_WASHING_TIME
};


FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

// interrupt service routine은 반드시 main함수 이전에 정의한다.
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; // 6 ~ 256으로 돌리기 위해
	if(timer_run) msec_count++;

	fnd_shoot = 1;
	ultrasonic_check_timer++; // 초음파센서에 활용할 타이머!
}



int main(void)
{
	init_fnd();
	init_timer0();
	init_uart0();
	init_ultrasonic();
	stdout = &OUTPUT;	
	sei();			
	
	int step = 0;
	int mode = 0;
	int washing_time = 0; // 지금 선택한 모드의 남은 세탁시간
	

	while(1)
	{
		if(fnd_shoot){
			display_2fnds(); // <- fnd에 보여줄거
		}
		if(msec_count >= 1000)
		{
			sec_count++;
			
			// 세탁 중일때만
			if(step == 2) washing_time = (washing_time < 1) ? 0 : washing_time - 1;
		}
		
		switch(step){
			case 0:
				// 세탁기가 동작하지 않고 있는 상태
				// button 3을 누르면 동작한다.
				if(get_button(BUTTON3, BUTTON3PIN))
				{
					step++; // 세탁모드 선택으로 이동
				}
				break;
			case 1:
				// 세탁모드 선택 중
				mode = step1_select();
				if(mode == -1)
				{
					// 아직 사용자가 모드확인을 안누른 상황
					// step에 변화가 있으면 안된다
				}else
				{
					// 사용자가 모드확인을 눌렀을 때
					// 다음 step으로 넘어간다.
					step++;
					washing_time = washing_machine[mode]; // 다음스텝으로 넘어감과 동시에 세탁시작
				}
				break;
			case 2:
				
				if(washing_time == 0)
				{
					// 세탁이 끝남!!
					step++;
					washing_time = washing_machine[mode]; // 세탁 종료 후 같은모드로 한번 더 돌리기 상황을 방지하여 다시 값 설정하기
				}
				step2_washing(mode, washing_time);
				break;
			case 3:
				step = step3_wash_end();
				if(step == -1)
				{
					// 아직 선택 전 -> step유지
					step = 3;
				}else
				{
					// 선택한 step으로 넘어간다
					mode = 0;
					washing_time = 0; // 변수 초기화
				}
				break;
		}
	}
	

	return 0;
}



// AVR의 8bit counter timer0를 초기화한다.
// 임베디드/FPGA에서 가장 중요한건 초기화 -> init함수는 특히 신경쓰기!!
void init_timer0(void)
{
	// 분주 (divider / prescale)
	// 16MHz / 64 down!!
	// 16000000Hz / 64 = 250,000 Hz 로 만든다!
	// T(주기) = 1clock에 걸리는 시간 = 1 / 250000 = 0.000004sec = 0.004ms (4us)
	// 8bit timer overflow 발생 시 소요시간 = 0.004ms * 256 = 1.024ms = 0.001024sec
	//	-> interrupt를 정확하게 1ms마다 오게 하고싶으면, 
	//    사용자 지정으로 256이 아니라 250클록마다 인터럽트가 생기도록
	
	// TCMT0 초기값 설정
	TCNT0 = 6; // 정확히 1ms를 세기 위해 초기값을 0이 아니라 6으로 해준다. (250 펄스 이후에 인터럽트)
	
	// 분주비 설정 (64)
	TCCR0 |= 1 << CS02 | 0 << CS01 | 0 << CS00;
	
	// timer overflow interrupt 허용 (TIMER0 OVF)
	TIMSK |= 1 << TOIE0;	// 지역 인터럽트 허용

	
}

int step1_select(void)
{
	
	static int result_mode = 0;
	// 사용자가 선택한 모드를 반환한다
	if(get_button(BUTTON0, BUTTON0PIN))
	{
		 // mode up
		 result_mode++;
		 result_mode %= MODE_NUM;
	}
	if(get_button(BUTTON1, BUTTON1PIN))
	{
		// mode down
		result_mode--;
		result_mode = (result_mode + MODE_NUM) % MODE_NUM;
	}
	if(get_button(BUTTON2, BUTTON2PIN))
	{
		// 세탁 시작 -> 초음파센서로 문이 닫혀있는지 확인하고 보내기
		
		// ------------
		// TO DO
		// 초음파센서 확인
		// -------------
		
		return result_mode;
	}
	return -1;
}

void step2_washing(int mode, int re_wash_time)
{
	// mode : 지금 어떤 모드인가
	// re_wash_time : 지금 상태에서 남은 세탁시간은 몇인가
	void (*wash_funcs[])(int) = {
		mode1_default,
		mode2_wool,
		mode3_blanket,
		mode3_dry
	};
	
	// 버튼 동작 확인
	// TO DO
	
	// mode에 맞추어 함수 실행
	wash_funcs[mode](re_wash_time);
}

int step3_wash_end(void)
{
	// 종료 노래를 재생한다
	// TO DO
	
	// 사용자 입력에 맞게, 종류 후 넘어갈 step을 반환한다
	if(get_button(BUTTON0, BUTTON0PIN))
	{
		// 세탁물을 꺼내었다 -> 세탁기 종료
		return 0;
	}
	if(get_button(BUTTON1, BUTTON1PIN))
	{
		// 같은 모드로 한번 더 돌린다
		return 2;
	}
	if(get_button(BUTTON2, BUTTON2PIN))
	{
		// 모드선택으로 되돌아간다
		return 1;	
	}
	return -1;
}

void mode1_default(int re_wash_time)
{
	// mode에 맞게 모터 회전속도를 조절한다.
	// TODO
}

void mode2_wool(int re_wash_time)
{
	// mode에 맞게 모터 회전속도를 조절한다.
	// TODO
}

void mode3_blanket(int re_wash_time)
{
	// mode에 맞게 모터 회전속도를 조절한다.
	// TODO
}

void mode3_dry(int re_wash_time)
{
	// mode에 맞게 모터 회전속도를 조절한다.
	// TODO
}