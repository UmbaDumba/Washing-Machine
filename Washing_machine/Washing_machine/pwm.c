/*
 * pwm.c
 *
 * Created: 2025-03-13 오후 12:48:56
 *  Author: microsoft
 */ 

#include "pwm.h"

int timer_run = 0;

extern volatile int fnd_shoot;
extern volatile int msec_count;
extern uint32_t sec_count;
extern volatile int circle_count;
extern int circle_dir;

/*
	16bit timer 3
	PWM
	=============
	PE3 : OC3A
	PE4 : OC3B <- external int 4 사용중 (초음파 echo펄스)
	PE5 : OC3C <- PWM 사용, 모터연결!
	============
	버튼제어
	BTN0 : start/stop
	BTN1 : speed up		(OC3C : 20 증가, max 250)
	BTN2 : speed down	(OC3C : 20 감소, min 60) motor stop 방지! (0으로 두지 말것)
*/
void init_timer3(void)
{
	DDRE |= 1 << 3 | 1 << 5; // 4번은 external int로 사용중이니까 건들지 말쟈
	
	// 모드설정 : 8비트 고속PWM
	TCCR3A |= 1 << WGM30;
	TCCR3B |= 1 << WGM31;
	
	// 비반전 모드 -> 비교일치 인터럽트가 생길 때 LOW로 바뀜
	TCCR3A |= 1 << COM3C1;
	
	// 분주비 : 64, 16000000Hz / 64 = 2500000Hz = 250KHz
	// T = 1 / 2500000Hz = 0.000004sec = 4us = 1클럭에 걸리는 시간
	// 즉 overflow가 발생할 때 (256번 count) 소요시간 1.024ms
	//							절반이 지났을 때 (125) 소요시간 0.5ms이다.
	//											->  소수점 날리면 127개로 치려나보당,,
	TCCR3B |= 0 << CS32 | 1 << CS31 | 1 << CS30;
	
	// PWM 값 초기화
	OCR3C = 0; // OCR : PWN값 초기화! 처음에는 LOW에서 시작하겠다는것
	
	// 비교일치 인터럽트 활성화
	//ETIMSK |= 1 << OCIE3C;
}

// PF6 : IN1
// PF7 : IN2
// PF6	PF7
//	0	 1		: 역회전
//	1	 0		: 정회전
//	1	 1		: stop     <- pwm에 0을 주어도 stop임!! (두가지 방법인것)

void init_L298N(void)
{
	MOTOR_DIR_DDR |= 1 << IN1 | 1 << IN2; // 출력모드
	MOTOR_DIR_PORT &= ~(1 << IN1 | 1 << IN2); // reset
	
	MOTOR_DIR_PORT |= 1 << IN1;		// 정회전
}

void motor_set_speed_num(int n)
{
	 // n으로 모터의 speed를 설정함
	 MOTOR_PWM = (n < MOTOR_SPEED_MIN) ? MOTOR_SPEED_MIN : 
			(n > MOTOR_SPEED_MAX) ? MOTOR_SPEED_MAX : n;
}

void motor_set_speed_max(void)
{
	// 모터 speed를 최대로 설정
	MOTOR_PWM = MOTOR_SPEED_MAX;
}

void motor_set_speed_min(void)
{
	// 모터 speed를 최소로 설정
	MOTOR_PWM = MOTOR_SPEED_MIN;
}

void motor_speed_up_num(int n)
{
	MOTOR_PWM = (MOTOR_PWM >= MOTOR_SPEED_MAX) ? MOTOR_SPEED_MAX : (MOTOR_PWM + n);
}

void motor_speed_up_20(void)
{
	motor_speed_up_num(20);
}

void motor_speed_down_num(int n)
{
	MOTOR_PWM = (MOTOR_PWM - n <= MOTOR_SPEED_MIN) ? MOTOR_SPEED_MIN : (MOTOR_PWM - n);
}

void motor_speed_down_20(void)
{
	motor_speed_down_num(20);
}

void motor_stop(void)
{
	MOTOR_PWM = MOTOR_STOP;
}

void pwm_fan_control_main(void)
{
	uint8_t	start_button = 0;
	
	init_button();
	init_timer3();
	
	while(1)
	{
		if(get_button(BUTTON0, BUTTON0PIN)) // change mode
		{
			start_button = !start_button;
			OCR3C = start_button ? 250 : 0;
			// 모드가 변경되었을 때 속도값 다 초기화시켜야됨!!
			// 250 : 회전 기본값 (최대), 0 : stop
		}
		else if(get_button(BUTTON1, BUTTON1PIN)) // speed up
		{
			OCR3C = (OCR3C >= 250) ? 250 : (OCR3C + 20);
		}
		else if(get_button(BUTTON2, BUTTON2PIN)) // speed down
		{
			OCR3C = (OCR3C <= 70) ? 60 : (OCR3C - 20);
		}
	}
}

// 방향제어 추가
void L298N_pwm_fan_control_main(void)
{
	uint8_t	start_button = 0;
	uint8_t forward = 1;			// 초기값 정회전
	
	init_button();
	init_timer3();
	init_L298N();
	
	while(1)
	{
		if(get_button(BUTTON0, BUTTON0PIN)) // change mode
		{
			start_button = !start_button;
			timer_run = !timer_run;
			OCR3C = start_button ? 250 : 0;
			// 모드가 변경되었을 때 속도값 다 초기화시켜야됨!!
			// 250 : 회전 기본값 (최대), 0 : stop
		}
		else if(get_button(BUTTON1, BUTTON1PIN)) // speed up
		{
			OCR3C = (OCR3C >= 250) ? 250 : (OCR3C + 20);
		}
		else if(get_button(BUTTON2, BUTTON2PIN)) // speed down
		{
			OCR3C = (OCR3C <= 70) ? 60 : (OCR3C - 20);
		}
		else if(get_button(BUTTON3, BUTTON3PIN))
		{
			forward = !forward;
			circle_dir = !circle_dir;
			
			PORTF &= ~(1 << 6 | 1 << 7); // 일단 reset;
			PORTF |= forward ? (1 << 6) : (1 << 7);
		}
		
		
	}
}