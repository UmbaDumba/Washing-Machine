/*
 * button.c
 *
 * Created: 2025-03-05 오후 2:30:27
 *  Author: microsoft
 */ 
#include "button.h"

int interrupt_arr[4] = {INT0, INT1, INT2, INT3};
int button_arr[4] = {BUTTON0, BUTTON1, BUTTON2, BUTTON3};
extern volatile int step;
volatile int is_button_pressed[4] = {0, 0, 0, 0};
extern void RRR();
	
ISR(INT0_vect){
	is_button_pressed[0] = 1;
}

ISR(INT1_vect){
	is_button_pressed[1] = 1;
	
}

ISR(INT2_vect){
	is_button_pressed[2] = 1;
}

ISR(INT3_vect){
	is_button_pressed[3] = 1;
}
                                                
void init_button(void)
{
	BUTTON_DDR &= ~( 1 << BUTTON0PIN | 1 << BUTTON1PIN | 1 << BUTTON2PIN | 1 << BUTTON3PIN);
}
// 예) 
// BUTTON0 : S/W 번호  --> button_num
// BUTTON0PIN : button_pin  
// 리턴값 : 1 :  버튼을 눌렀다 떼면 1을 return 
//          0 :  ide 

int get_button(int button_num, int button_pin)
{
	static unsigned char button_status[BUTTON_NUMER] =
	{BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE,BUTTON_RELEASE};	
	// 	지역 변수에 static을 쓰면 전역 변수처럼 함수를 빠져 나갔다 다시 들어 와도 값을 유지 한다.  
	int currtn_state;
	
	currtn_state = BUTTON_PIN & (1 << button_pin);   // 버튼을 읽는다. 
	if (currtn_state && button_status[button_num] == BUTTON_RELEASE)  // 버튼이 처음 눌려진 noise high 
	{
		_delay_ms(60);   // noise가 지나가기를 기다린다. 
		button_status[button_num] = BUTTON_PRESS;   // noise가 지나간 상태의 High 상태 
		return 0;   // 아직은 완전히 눌렸다 떼어진 상태가 아니다. 
	}
	else if (currtn_state==BUTTON_RELEASE && button_status[button_num] == BUTTON_PRESS)
	{
		_delay_ms(60);
		button_status[button_num] = BUTTON_RELEASE;   // 다음 버튼 체크를 위해서 초기화
		return 1;   // 완전히 1번 눌렸다 떼어진 상태로 인정		
	}
	
	return 0;   // 버튼이 open상태 
}

void init_button_interrupt(void){
	for (int i = 0; i < BUTTON_NUMER; i++){
		BUTTON_DDR &= ~(1 << button_arr[i]);
	}
	EICRA |= 1 << ISC01 | 1 << ISC11 | 1 << ISC21 | 1 << ISC31;
	EICRA &= ~(1 << ISC00 | 1 << ISC10 | 1 << ISC20 | 1 << ISC30);
	
	for (int i = 0; i < BUTTON_NUMER; i++){
		enable_button_interrupt(button_arr[i]);
	}
}

void enable_button_interrupt(int button_num){
	EIMSK |= (1 << interrupt_arr[button_num]); 
}

void disable_button_interrupt(int button_num){
	EIMSK &= ~(1 << interrupt_arr[button_num]); 
}

int is_pressed_button(int num){
	if (is_button_pressed[num]){
		is_button_pressed[num] = 0;
		return 1;
	} 
	return is_button_pressed[num];
}