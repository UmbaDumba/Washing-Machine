/*
 * led.c
 *
 * Created: 2025-03-05 오전 10:21:53
 *  Author: microsoft
 */ 



#include "led.h"

extern volatile int msec_count; // extern으로 되어있으면 초기화 뺄것!
int led_state = 0;
int led_arr[LED_COUNT] = {LED1, LED2, LED3, LED4, LED5, LED6};

void (*funcs[FUNC_NUM])(void) =
{
	led_all_on,
	led_all_off,
	shift_left_ledon,		
	shift_right_ledon,
	shift_left_keep_ledon,	
	shift_right_keep_ledon,
	flower_on,				
	flower_off
};

int led_main(void)   // 정의 
{
	led_init();
	led_all_on();
	_delay_ms(1000);
	led_all_off();
	_delay_ms(1000);
	
	//DDRA = 0xff;   // PORTA에 연결된 pin8개를 output mode로 

	//funcs[led_state]();
		
}

void led_init(void)
{
	for (int i = 0; i < LED_COUNT; i++){
		LED_DDR |= 1 << led_arr[i];
	}
}

void state_transaction(void)
{
	led_state++;
	led_state%=FUNC_NUM;
}

void flower_on(void)
{
	static int i = 0;
	
	if(msec_count >= DELAY){
		msec_count = 0;
		if(i >= 4){
			i = 0;
			PORTA = 0;
			state_transaction();
		}else{
			PORTA |= 0x10 << i | 0x08 >> i++;
		}
	}
}

void flower_off(void)
{
	unsigned char h=0xf0;
	unsigned char l=0x0f;
	static int i = 0;
	
	if(msec_count >= DELAY){
		msec_count = 0;
		if(i >= 4){
			i = 0;
			PORTA = 0;
			state_transaction();
		}else{
			PORTA = (h >> i) & 0xf0 | (l << i++) & 0x0f;
		}
	}
}

void shift_left_keep_ledon(void)
{
	static int i = 0;
	
	if(msec_count >= DELAY){
		msec_count = 0;
		if(i >= 8){
			i = 0;
			PORTA = 0;
			state_transaction();
		}else{
			PORTA |= 0b00000001 << i++;
		}
	}
}
void shift_right_keep_ledon(void)
{
	static int i = 0;
	if(msec_count >= DELAY){
		msec_count = 0;
		if(i >= 8){
			i = 0;
			PORTA = 0;
			state_transaction();
		}else{
			PORTA |= 0b10000000 >> i++;
		}
	}	
}
void shift_left_ledon(void)
{
	static int i = 0;
	
	if(msec_count >= DELAY){
		msec_count = 0;
		if(i >= 8){
			i = 0;
			PORTA = 0;
			state_transaction();
		}else{
			PORTA = 0b00000001 << i++;
		}
	}

}

void shift_right_ledon(void)
{
	static int i = 0;
	if(msec_count >= DELAY){
		msec_count = 0;
		if(i >= 8){
			i = 0;
			PORTA = 0;
			state_transaction();
		}else{
			PORTA = 0b10000000 >> i++;
		}
	}
}

void led_all_on(void)
{
	for (int i = 0; i < LED_COUNT; i++){
		LED_PORT |= 1 << led_arr[i];
	}
}

void led_all_off(void)
{
	for (int i = 0; i < LED_COUNT; i++){
		LED_PORT &= ~(1 << led_arr[i]);
	}
}

void make_pwm_led_control(void)
{
	int dim = 0; // led 밝기 조절
	int direction = 1;  //  1 : 밝기 증가
						// -1 : 밝기 감소
	DDRA = 0xff;
	PORTA = 0xff; // 일단 전부 켜두기
	
	// dark ---> bright ---> dark 반복
	
	while(1)
	{
		led_on_pwm(dim);
		dim += direction;
		
		if(dim == 0)	direction = 1;
		if(dim == 256)	direction = -1;
	}
}

// 한번의 사이클 (주기) 에서 led동작
void led_on_pwm(int dim)
{
	PORTA = 0xff;
	
	for(int i = 0; i < 256; i++)
	{
		if(i > dim) PORTA = 0x00;
		_delay_us(20);
	}	
}

void led_on_range(int led_count){
	for (int i = 0; i < led_count; i++){
		LED_PORT &= ~(1 << led_arr[i]);
	}
}