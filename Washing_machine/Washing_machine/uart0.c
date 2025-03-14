/*
 * uart0.c
 *
 * Created: 2025-03-10 오전 10:32:53
 *  Author: microsoft
 */ 

#include "uart0.h"
#include "led.h"



extern volatile int led_select;

/*
PC omportmaster로 부터 1byte가 들어오면 (즉, stop bit가 감지되면) RX complete인터럽트가 호출된다
== 한 바이트마다 인터럽트 처리를 해야하는것!
*/

ISR(USART0_RX_vect)
{
	volatile uint8_t rx_data;
	volatile static int i = 0;	// buffer의 index
	
	rx_data = UDR0; // uart0 하드웨어로 들어온 문자 읽어오기
					// 이렇게 assign연산자로 UDR0값을 읽어오는 순간, UDR0는 empty가됨 (자동)
	
	if(rx_data == '\n')
	{
		rx_buff[rear++][i] = 0; // 문장의 끝은 항상 0
		rear %= COMMAND_NUMBER; // 원형큐
		i = 0; // 새로오는건 버퍼에 새롭게 채워야함 (다른 메시지니까!)
		
		// 큐 full 여부 확인
	}
	else
	{
		// 그냥 버퍼에 넣기 (push)
		rx_buff[rear][i++] = rx_data;
		// command length 확인
	}
}


/*
[init에 고려해야할것]

전송속도 : 9600bps -> 1초에 960자를 받음
				1글자 송수신에 약 1ms가 걸린다 (1글자 == 10비트)

비동기 방식 : start, stop 비트의 활용

RX : interrupt (받는것)
TX : polling (보내는것)

*/
void init_uart0(void)
{
	// 전송속도 설정
	UBRR0H = 0x00;
	UBRR0L = 207; // 2배속 모드임
	//UBRR0L = 16;
	//UBRR0L = 832;
	
	UCSR0A |= 1 << U2X0; // 2배속 통신
	
	UCSR0C |= 0x06; // 비동기, none 8bit, data 8 bit
	
	// 인터럽트, uart 기능 설정
	//UCSR0B |= 1 << RXEN0; // 수신 허용
	//UCSR0B |= 1 << TXEN0; // 송신 허용
	//UCSR0B |= 1 << RXCIE0; // 수신 인터럽트 허용
	UCSR0B |= 1 << RXEN0 | 1 << TXEN0 | 1 << RXCIE0;
	
	rear = front = 0;
}

void UART0_transmit(uint8_t data)
{
	// 데이터가 전송중이면, 끝날때까지 기다려야함
	// == 레지스터가 비었는지 안비었는지 확인해양함
	while(!(UCSR0A & 1 << UDRE0)) ; // 버퍼가 비어있을때까지 여기서 무한대기
	
	UDR0 = data; // data를 h/w에 전송
}

void pc_command_processing(void)
{
	if(front != rear)
	{
		printf("%s\n", (char *)rx_buff[front]); 
		// warning 방지였다!
		
		if(strncmp(rx_buff[front], "led_all_on", strlen("led_all_on")) == NULL)
		{
			printf(("find: led_all_on\n"));
			led_select = 0;
		}
		else if(strncmp(rx_buff[front], "led_all_off", strlen("led_all_off")) == NULL)
		{
			printf(("find: led_all_off\n"));
			led_select = 1;
		}
		else if(strncmp(rx_buff[front], "shift_left_ledon", strlen("shift_left_ledon")) == NULL)
		{
			printf(("find: shift_left_ledon\n"));
			led_select = 2;
		}
		else if(strncmp(rx_buff[front], "shift_right_ledon", strlen("shift_right_ledon")) == NULL)
		{
			printf(("find: shift_right_ledon\n"));
			led_select = 3;
		}
		else if(strncmp(rx_buff[front], "shift_left_keep_ledon", strlen("shift_left_keep_ledon")) == NULL)
		{
			printf(("find: shift_left_keep_ledon\n"));
			led_select = 4;
		}
		else if(strncmp(rx_buff[front], "shift_right_keep_ledon", strlen("shift_right_keep_ledon")) == NULL)
		{
			printf(("find: shift_right_keep_ledon\n"));
			led_select = 5;
		}
		else if(strncmp(rx_buff[front], "flower_on", strlen("flower_on")) == NULL)
		{
			printf(("find: flower_on\n"));
			led_select = 6;
		}
		else if(strncmp(rx_buff[front], "flower_off", strlen("flower_off")) == NULL)
		{
			printf(("find: flower_off\n"));
			led_select = 7;
		}
		
		front++;
		front %= COMMAND_NUMBER;
	}
}