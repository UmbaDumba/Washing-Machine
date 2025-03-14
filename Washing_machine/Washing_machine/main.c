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

typedef struct{
	int mode;
	int washing_time;
	int washing_start_time;
	int washing_end_time;
}washing_info;

// tnd_data에 들어갈 수 있는것
// 시간정보, stop 텍스트, circle 등등

// extern----------------------------------------------------------

extern volatile uint8_t rx_message_received;

extern uint32_t sec_count;
extern int timer_run;

// 선언 ----------------------------------------------------
void init_timer0(void);
void mode1_default(int re_wash_time);
void mode2_wool(int re_wash_time);
void mode3_blanket(int re_wash_time);
void mode4_dry(int re_wash_time);


volatile int msec_count = 0;
volatile int ultrasonic_check_timer = 0;
volatile int is_washing = 0;
volatile int washing_msec_count;
uint32_t washing_sec_time = 0;


volatile int fnd_shoot = 0;

int washing_machine[] = {    // 각 모드별 동작시간
	MODE_1_WASHING_TIME, 
	MODE_2_WASHING_TIME, 
	MODE_3_WASHING_TIME,
	MODE_4_WASHING_TIME
};

void (*wash_funcs[])(int) = {
	mode1_default,
	mode2_wool,
	mode3_blanket,
	mode4_dry
};

int wash_speeds[][2] = {
	{
		WASHING_SPEED_DEFAULT, MOTOR_SPEED_MAX
	},
	{
		WASHING_SPEED_DEFAULT, MOTOR_SPEED_MIN
	},
	{
		MOTOR_SPEED_MAX, MOTOR_SPEED_MAX
	},
	{
		MOTOR_SPEED_MAX, MOTOR_SPEED_MIN
	}
};

FILE OUTPUT = FDEV_SETUP_STREAM(UART0_transmit, NULL, _FDEV_SETUP_WRITE);

// interrupt service routine은 반드시 main함수 이전에 정의한다.
ISR(TIMER0_OVF_vect)
{
	TCNT0 = 6; // 6 ~ 256으로 돌리기 위해
	msec_count++;
	if(is_washing) washing_msec_count++;

	fnd_shoot = 1;
	ultrasonic_check_timer++; // 초음파센서에 활용할 타이머!
}

void test_main(){
	int step = 2;
	int mode = 0;
	int washing_time = 0; // 지금 선택한 모드의 남은 세탁시간
	

	while(1)
	{
	
		if(get_button(BUTTON0, BUTTON0PIN))
		{
			
			mode++;
			mode %= MODE_NUM;
		}
	}
}

void init_washtime(void){
	is_washing = 0;
	washing_msec_count = 0;
	washing_sec_time = 0;
}


int main(void)
{
	init_fnd();
	init_timer0();
	init_button_interrupt();
	init_timer3();
	init_uart0();
	init_ultrasonic();
	init_L298N();
	stdout = &OUTPUT;	
	sei();			
	
	//test_main();
	//L298N_pwm_fan_control_main();
	
	int step = 0;
	int mode = 0;
	int isOn = 0;
	
	while(1)
	{
		//if (!isOn) continue;
		
		
		if(msec_count >= 1000)
		{
			sec_count++;
			msec_count = 0;
		}
		
		if(washing_msec_count >= 1000)
		{
			washing_msec_count = 0;
			washing_sec_time++;
		}
		
		// 버튼 3번은 항상 step 1 로 초기화
		if (is_pressed_button(BUTTON3)){
			mode = 0;
			isOn = !isOn;
			step = isOn ? 1 : 0;
		}
		
		switch(step){
			case 0:
				// 세탁기 off 상태
				fnd_display_dashs();
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
					is_washing = 1;
					step++;
				}
				break;
			case 2:
				fnd_display_min_sec_num(washing_machine[mode] - washing_sec_time);
				_delay_ms(1);
				fnd_display_big_circle();
				_delay_ms(1);
				if(washing_machine[mode] - washing_sec_time == 0)
				{
					// 세탁이 끝남!!
					step++;
					init_washtime();
				}
				
				if (is_pressed_button(BUTTON0)){
					is_washing = 0;
				}
				if (is_pressed_button(BUTTON1)){
					is_washing = 1;
				}
				if (is_pressed_button(BUTTON2)){
					init_washtime();
					step--;
				}
				if(is_washing) wash_funcs[mode](washing_sec_time);
				else motor_stop();
				
				break;
			case 3:
				//step = step3_wash_end();
				fnd_display_done();
				_delay_ms(1);
				if (is_pressed_button(BUTTON0)){
					init_washtime();
					step = 0;
				}
				
				if (is_pressed_button(BUTTON1)){
					step = 2;
					is_washing = 1;
				}
				
				if (is_pressed_button(BUTTON2)){
					step = 1;
					mode = 0;
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
	if(is_pressed_button(BUTTON0))
	{
		 // mode up
		 result_mode++;
		 result_mode %= MODE_NUM;
	}
	if(is_pressed_button(BUTTON1))
	{
		// mode down
		result_mode--;
		result_mode = (result_mode + MODE_NUM) % MODE_NUM;
	}
	
	fnd_display_selectn(result_mode);
	_delay_ms(1);
	
	if(is_pressed_button(BUTTON2))
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
	motor_set_speed_num(wash_speeds[0][re_wash_time % 2]);
	//motor_set_speed_num(250);
}

void mode2_wool(int re_wash_time)
{
	// mode에 맞게 모터 회전속도를 조절한다.
	// TODO
	motor_set_speed_num(wash_speeds[1][re_wash_time % 2]);
	//motor_set_speed_num(250);
}

void mode3_blanket(int re_wash_time)
{
	// mode에 맞게 모터 회전속도를 조절한다.
	// TODO
	motor_set_speed_num(wash_speeds[2][re_wash_time % 2]);
	//motor_set_speed_num(250);
}

void mode4_dry(int re_wash_time)
{
	// mode에 맞게 모터 회전속도를 조절한다.
	// TODO
	motor_set_speed_num(wash_speeds[3][re_wash_time % 2]);
	//motor_set_speed_num(250);
}