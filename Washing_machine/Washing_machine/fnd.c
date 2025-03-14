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

int fnd_main(void)
{
	init_fnd();
	
	while (1)
	{
		fnd_display();
		_delay_ms(1);
		fnd_display_circle();
		_delay_ms(1);
		
	}
	
	return 0;
}

void display_stop(void){
	
	static int digit_select = 0; // 오른쪽부터 3210으로 보기
	
	clear_digit();
	switch(digit_select)
	{
		case 0:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D4; 
			FND_DATA_PORT = 0x8c; 
			break;
		case 1:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D3;
			FND_DATA_PORT = 0xc0;
			break;
		case 2:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D2;
			FND_DATA_PORT = 0xf8; 
			break;
		case 3:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D1;
			FND_DATA_PORT = 0x92; 
			break;
		case 4:
			FND_DIGIT_PORT2 |= 1 << FND_DIGIT_D8;
			FND_DATA_PORT = 0x8c;
			break;
		case 5:
			FND_DIGIT_PORT2 |= 1 << FND_DIGIT_D7;
			FND_DATA_PORT = 0xc0;
			break;
		case 6:
			FND_DIGIT_PORT2 |= 1 << FND_DIGIT_D6;
			FND_DATA_PORT = 0xf8;
			break;
		case 7:
			FND_DIGIT_PORT2 |= 1 << FND_DIGIT_D5;
			FND_DATA_PORT = 0x92;
			break;
	}
	digit_select++;
	digit_select%=8;
}

void display_2fnds(void){
	static int state = 0;
	
	if(state < 4) fnd_display();
	else fnd_display_circle();
	
	state++;
	state%=8;
}

void clear_digit(void)
{
	FND_DIGIT_PORT &= ~(1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4); 
	FND_DIGIT_PORT2 &= ~(1 << FND_DIGIT_D5 | 1 << FND_DIGIT_D6 | 1 << FND_DIGIT_D7 | 1 << FND_DIGIT_D8); 
}

void fnd_display_circle(void)
{
	uint8_t fnd_0[][12] = {
		{
			0xfe, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
			0xfe, 0xfe, 0xfe, 0xf6, 0xf2, 0xf0
		},
		{
			0xfd, 0xf9, 0xf1, 0xf1, 0xf1, 0xf1, 
			0xf1, 0xf1, 0xf1, 0xf1, 0xf1, 0xf0
		}
	};
	
	uint8_t fnd_1[][12] = {
		{
			0xff, 0xfe, 0xfe, 0xfe, 0xfe, 0xfe,
			0xfe, 0xfe, 0xf6, 0xf6, 0xf6, 0xf6
		},
		{
			0xff, 0xff, 0xff, 0xf7, 0xf7, 0xf7, 
			0xf7, 0xf7, 0xf7, 0xf7, 0xf6, 0xf6
		}
	};
	
	uint8_t fnd_2[][12] = {
		{
			0xff, 0xff, 0xfe, 0xfe, 0xfe, 0xfe,
			0xfe, 0xf6, 0xf6, 0xf6, 0xf6, 0xf6
		},
		{
			0xff, 0xff, 0xff, 0xff, 0xf7, 0xf7, 
			0xf7, 0xf7, 0xf7, 0xf6, 0xf6, 0xf6
		}
	};
	
	uint8_t fnd_3[][12] = {
		{
			0xff, 0xff, 0xff, 0xfe, 0xde, 0xce,
			0xc6, 0xc6, 0xc6, 0xc6, 0xc6, 0xc6
		},
		{
			0xff, 0xff, 0xff, 0xff, 0xff, 0xf7, 
			0xe7, 0xc7, 0xc6, 0xc6, 0xc6, 0xc6
		}
	};
	
	static int digit_select = 0; // 3210
	int mycount = msec_count + (sec_count % 10 * 1000);
	int speed = 30000 / OCR3C;
	
	
	
	clear_digit();
	switch(digit_select)
	{
		case 0:
			FND_DIGIT_PORT2 |= 1 << FND_DIGIT_D8; 
			FND_DATA_PORT = fnd_0[circle_dir][mycount / speed % 12]; 
			break;
		case 1:
			FND_DIGIT_PORT2 |= 1 << FND_DIGIT_D7;
			FND_DATA_PORT = fnd_1[circle_dir][mycount / speed % 12];
			break;
		case 2:
			FND_DIGIT_PORT2 |= 1 << FND_DIGIT_D6;
			FND_DATA_PORT = fnd_2[circle_dir][mycount / speed % 12];
			break;
		case 3:
			FND_DIGIT_PORT2 |= 1 << FND_DIGIT_D5;
			FND_DATA_PORT = fnd_3[circle_dir][mycount / speed % 12];
			break;
	}
	
	
	digit_select++;
	digit_select%=4;
	
	
}


void fnd_display(void)
{
	// common-anode
	uint8_t fnd_font[] = {
		0xc0, // 0
		0xf9, // 1
		0xa4, // 2
		0xb0, // 3
		0x99, // 4
		0x92, // 5
		0x82, // 6
		0xd8, // 7
		0x80, // 8
		0x98, // 9
		0x7f  // .
	};
	
	static int digit_select = 0; // 오른쪽부터 3210으로 보기 (0,1 초 / 3,2 분)
	static int dp_state = 0;
	
	clear_digit();
	switch(digit_select)
	{
		case 0:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D4; // 공통핀을 1로 만들기 -> 자릿수 선택하는것!!
			FND_DATA_PORT = fnd_font[sec_count % 10]; // 초의 1단위
			break;
		case 1:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D3;
			if(sec_count % 2 == 0){
				FND_DATA_PORT = fnd_font[sec_count / 10 % 6];
				}else{
				FND_DATA_PORT = fnd_font[sec_count / 10 % 6] & fnd_font[10];
			}
			break;
		case 2:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D2;
			FND_DATA_PORT = fnd_font[sec_count / 60 % 10]; // 분의 1단위
			break;
		case 3:
			FND_DIGIT_PORT |= 1 << FND_DIGIT_D1;
			FND_DATA_PORT = fnd_font[sec_count / 600 % 6]; // 분의 10단위 (60분을 넘어가는 부분을 잘라야함!!)
			break;
	}
	digit_select++;
	digit_select%=4;
}



void init_fnd(void)
{
	FND_DATA_DDR = 0xff;  // 출력 모드로 설정
	
	// FND_DIGIT_DDR |= 0xf0;   // 자릿수 선택 7654 
	FND_DIGIT_DDR |= 1 << FND_DIGIT_D1 | 1 << FND_DIGIT_D2 | 1 << FND_DIGIT_D3 | 1 << FND_DIGIT_D4; 
	FND_DIGIT_DDR2 |= 1 << FND_DIGIT_D5 | 1 << FND_DIGIT_D6 | 1 << FND_DIGIT_D7| 1 << FND_DIGIT_D8;
	
	// fnd를 all off 
	FND_DATA_PORT = ~0x00;
}