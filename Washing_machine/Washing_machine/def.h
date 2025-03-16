/*
 * def.h
 *
 * Created: 2025-03-10 오후 3:05:46
 *  Author: microsoft
 */ 


#ifndef DEF_H_
#define DEF_H_


#define COMMAND_NUMBER 10 // command 개수
#define COMMAND_LENGTH 40 // command 길이

#define FUNC_NUM 8 // led함수 개수

#define STEP_0_MACHINE_OFF	0		// 각 step 상수
#define STEP_1_SELECT_MODE	1
#define STEP_2_WASHING		2
#define STEP_3_DONE			3

#define MODE_NUM		4 // 세탁모드 개수

#define MODE_1_DEFAULT	0
#define MODE_2_WOOL		1
#define MODE_3_BLANKETS	2
#define MODE_4_DRY		3

#define MODE_1_WASHING_TIME	7	// 각 모드별 동작시간 (단위 : 초)
#define MODE_2_WASHING_TIME	10
#define MODE_3_WASHING_TIME	15
#define MODE_4_WASHING_TIME	7

#define WASHING_SPEED_DEFAULT 150

#endif /* DEF_H_ */