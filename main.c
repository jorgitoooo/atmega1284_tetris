#include <avr/io.h>
//#include <stdlib.h>
//#include <string.h>

#include "timer.h"
//#include "io.c"
#include "MAX7219.c"

#include "consts.h"
#include "utils.c" // Init_LED_Matrices, Set_Brightness, Load_word, ...
#include "blocks.c" // Block drawing funcs
#include "block_ops.c"
#include "eeprom.h"
//#include "state_machines.h"

typedef unsigned  char uc;
typedef   signed  char sc;
typedef unsigned short us;

#define START_BTN    (~PINA & 0x01)
#define END_GAME_BTN (~PINA & 0x10)

//******************* GLOBALS
uc start = 0;
//uc curr_matrix = 0;
//sc from_bottom = 7;
//uc collision = 0;
uc game_over = 0;
us secs = 0;
uc can_write = 0;
//Block blocks[NUM_OF_BLOCKS];
//uc block_type;
//us secs = 0;
//******************* STATES
#define TS_ONE_MIN  60
#define TS_PERIOD    5
#define GP_PERIOD    5
#define SG_PERIOD    1
#define TASKS_PERIOD 1

//**************************** TIMER TICK

enum TimerStates { TS_Wait, TS_Timer, TS_Stop};
uc TimerTick(uc state)
{
	static us count = 0;
	switch(state)
	{
		case TS_Wait:
			state = start ? TS_Timer : TS_Wait;
			secs = 0;
			count = 0;
			break;
		case TS_Timer:
			if (count >= 2)
			{
				secs++;
				count = 0;
			}
			else
				count++;
			
			state = game_over ? TS_Stop : TS_Timer;
			break;
		case TS_Stop:
			can_write = 1;

			state = TS_Stop;
			break;
		default: state = TS_Wait; break;
	}
	return state;
}
//************************** START GAME TICK

enum StartGameStates { SG_Wait, SG_Start_Btn_Pressed, SG_Start };
uc StartGameTick(uc state)
{
	switch(state)
	{
		case SG_Wait:
			state = START_BTN ? SG_Start_Btn_Pressed : SG_Wait;
			break;
		case SG_Start_Btn_Pressed:
			state = START_BTN ? SG_Start_Btn_Pressed : SG_Start;
			break;
		case SG_Start:
			start = 1;
			state = SG_Wait;
			break;
		default: state = SG_Wait;
	}
	return state;
}

//***************** TASKS
#define NUM_TASKS 2
const uc num_of_tasks = NUM_TASKS;
typedef struct Task {
	uc elapsedTime;
	uc period;
	uc state;
	uc (*tick)(uc);
} Task;
Task tasks[NUM_TASKS];

void Initialize_Tasks(Task tasks[])
{
	uc task = 0;
	tasks[task].state = TS_Wait;
	tasks[task].period = TS_PERIOD;
	tasks[task].elapsedTime = 0;
	tasks[task].tick = &TimerTick;
	++task;
	tasks[task].state = SG_Wait;
	tasks[task].period = SG_PERIOD;
	tasks[task].elapsedTime = 0;
	tasks[task].tick = &StartGameTick;
	++task;
	/*
	tasks[task].state = GP_Init;
	tasks[task].period = GP_PERIOD;
	tasks[task].elapsedTime = 0;
	tasks[task].tick = &GamePlayTick;
	++task;
	*/
}
//************* TIMER ISR
void TimerISR()
{
	TimerFlag = 1;
	
	uc i;
	for (i = 0; i < NUM_TASKS; ++i) {
		if (tasks[i].elapsedTime >= tasks[i].period)
		{
			tasks[i].state = tasks[i].tick(tasks[i].state);
			tasks[i].elapsedTime = 0;
		}
		tasks[i].elapsedTime += TASKS_PERIOD;
	}
}

//************ MAIN
int main(void)
{
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;

	Init_LED_Matrices(4);
	Set_Brightness(0, 4);
	Clear_All();
	
	Initialize_Tasks(tasks);

	uc curr_matrix = 0;
	sc from_bottom = 7;
	uc collision = 0;

	Block blocks[num_of_blocks];
	Initialize_Blocks(blocks);

	uc block_type = rand() % 7;

	TimerSet(100);
	TimerOn();
	
	uc timer = 0;
	uc timer_limit = 1;
	
	PORTB = EEPROM_read(0);
	
	while (1) 
    {
		if (END_GAME_BTN)
			timer_limit = 0;
			
		while(!collision && !Game_Over() && start)
		{
			++timer;
			if (timer > timer_limit)
			{
				timer = 0;
				Clear_All();
				Draw_Block_Matrix();
				Drop_A_Block(
					&curr_matrix,
					&collision,
					&blocks[block_type],
					&from_bottom
				);
			}
			
			while(!TimerFlag);
			TimerFlag = 0;
		}
		
		if (collision && !Game_Over())
		{
			block_type = rand() % 8;
			collision = 0;
			curr_matrix = 0;
			from_bottom = 7;
			
			blocks[block_type].left  = blocks[block_type].init_left;
			blocks[block_type].right = blocks[block_type].init_right;
			blocks[block_type].orientation = Vertical_Up;
			
			Solid_Row_Eliminator();
			Draw_Block_Matrix();
		}
		else if (Game_Over() && !game_over && !can_write)
		{
			game_over = 1;
			Clear_All();
		}
		else if (can_write && game_over)
		{
			EEPROM_write(0, (uc)secs); // Writes the number of seconds played
			PORTB = (EEPROM_read(0));
		}
    }
}