/*
 * fnd.c
 *
 * Created: 2025-03-06 오후 12:23:41
 *  Author: microsoft
 */ 
#include "fnd.h"

extern volatile int msec_count;

//uint32_t ms_count=0;  // ms를 재는 count변수 unsigned int --> uint32_t
uint32_t sec_count=0;  // 초를 재는 count변수 unsigned int --> uint32_t
int circle_dir = 0;

// -------------------------------------------------------------------------
// fnd font

uint8_t fnd_num_font[] = {
	0x03, // 0
	0x9f, // 1
	0x25, // 2
	0x0d, // 3
	0x99, // 4
	0x49, // 5
	0x41, // 6
	0x1b, // 7
	0x01, // 8
	0x19, // 9
	0xfe  // .
};

// select 0n
uint8_t fnd_select_font[] = {
	0x49, // S
	0x61, // E
	0xe3, // L
	0x61, // E
	0x63, // C
	0x73, // T
	0x03  // 0
};

// donE
uint8_t fnd_donE_font[] = {
	0x85, // d
	0xc5, // o
	0xd5, // n
	0x61  // E
};

// STOP
uint8_t fnd_stop_font[] = {
	0x49, // S
	0x1f, // T
	0x03, // O
	0x31  // P
};

// - 
uint8_t fnd_dash_font = 0xfd;



// --------------------------------------------------------------------------

int digits[] = {FND_DIGIT_D1, FND_DIGIT_D2, FND_DIGIT_D3, FND_DIGIT_D4, FND_DIGIT_D5, FND_DIGIT_D6, FND_DIGIT_D7, FND_DIGIT_D8};
	

	
void fnd_display_done(void)
{
	static int digit_select = 0;
	
	fnd_clear_digit();
	FND_DIGIT_PORT |= 1 << digits[digit_select];
	FND_DATA_PORT = fnd_donE_font[digit_select % 4];
	
	digit_select++;
	digit_select%=8;
}

void fnd_display_selectn(int mode)
{
	static int digit_select = 0;
	
	fnd_clear_digit();
	
	FND_DIGIT_PORT |= 1 << digits[digit_select];
	if(digit_select < 7)	FND_DATA_PORT = fnd_select_font[digit_select];
	else FND_DATA_PORT = fnd_num_font[mode];

	digit_select++;
	digit_select%=8;
}

void fnd_display_dashs(void)
{
	static int digit_select = 0;
	
	fnd_clear_digit();
	FND_DIGIT_PORT |= 1 << digits[digit_select];
	FND_DATA_PORT = fnd_dash_font;
	
	digit_select++;
	digit_select%=8;
}

void fnd_display_stop(void)
{
	static int digit_select = 0;
	
	fnd_clear_digit();
	FND_DIGIT_PORT |= 1 << digits[digit_select];
	FND_DATA_PORT = fnd_stop_font[digit_select % 4];
	
	digit_select++;
	digit_select%=8;
}

void fnd_display_min_sec_num(int n)
{
	// n : sec
	static int digit_select = 0; // 오른쪽부터 3210으로 보기 (0,1 초 / 3,2 분)
	static int dp_state = 0;
	
	fnd_clear_digit();
	switch(digit_select)
	{
		case 0:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D4; // 공통핀을 1로 만들기 -> 자릿수 선택하는것!!
			FND_DATA_PORT = fnd_num_font[n % 10]; // 초의 1단위
			break;
		case 1:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D3;
			FND_DATA_PORT = fnd_num_font[n / 10 % 6];
			break;
		case 2:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D2;
			FND_DATA_PORT = fnd_num_font[n / 60 % 10]; // 분의 1단위
			break;
		case 3:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D1;
			FND_DATA_PORT = fnd_num_font[n / 600 % 6]; // 분의 10단위 (60분을 넘어가는 부분을 잘라야함!!)
			break;
	}
	digit_select++;
	digit_select%=4;
}

void fnd_display_min_sec(void)
{
	
	
	static int digit_select = 0; // 오른쪽부터 3210으로 보기 (0,1 초 / 3,2 분)
	static int dp_state = 0;
	
	fnd_clear_digit();
	switch(digit_select)
	{
		case 0:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D4; // 공통핀을 1로 만들기 -> 자릿수 선택하는것!!
			FND_DATA_PORT = fnd_num_font[sec_count % 10]; // 초의 1단위
			break;
		case 1:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D3;
			if(sec_count % 2 == 0){
				FND_DATA_PORT = fnd_num_font[sec_count / 10 % 6];
				}else{
				FND_DATA_PORT = fnd_num_font[sec_count / 10 % 6] & fnd_num_font[10];
			}
			break;
		case 2:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D2;
			FND_DATA_PORT = fnd_num_font[sec_count / 60 % 10]; // 분의 1단위
			break;
		case 3:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D1;
			FND_DATA_PORT = fnd_num_font[sec_count / 600 % 6]; // 분의 10단위 (60분을 넘어가는 부분을 잘라야함!!)
			break;
	}
	digit_select++;
	digit_select%=4;
}

void fnd_clear_digit(void)
{
	FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4 | 1 << FND_DIGIT_D5 | 1 << FND_DIGIT_D6 | 1 << FND_DIGIT_D7 | 1 << FND_DIGIT_D8);

}

void init_fnd(void)
{
	FND_DATA_DDR = 0xff;  // 출력 모드로 설정
	
	// FND_DIGIT_DDR |= 0xf0;   // 자릿수 선택 7654 
	FND_DIGIT_DDR |= 1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4
					| 1 << FND_DIGIT_D5 | 1 << FND_DIGIT_D6 | 1 << FND_DIGIT_D7| 1 << FND_DIGIT_D8;
	
	// fnd를 all off 
	FND_DATA_PORT = ~0x00;
}