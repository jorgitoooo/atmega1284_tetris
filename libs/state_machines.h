#ifndef __STATE_MACHINES_H__
#define __STATE_MACHINES_H__

#define START_BTN (~PINA & 0x01)

typedef unsigned  char uc;
typedef   signed  char sc;
typedef unsigned short us;

//******************* GLOBALS
uc start = 0;
//uc curr_matrix = 0;
//sc from_bottom = 7;
//uc collision = 0;
uc game_over = 0;
//Block blocks[NUM_OF_BLOCKS];
//uc block_type;
//us secs = 0;
//******************* STATES
#define TS_ONE_MIN  60
#define TS_PERIOD    2
#define GP_PERIOD    5
#define SG_PERIOD    1
#define TASKS_PERIOD 1

//**************************** TIMER TICK

enum TimerStates { TS_Wait, TS_Timer, TS_Stop};
uc TimerTick(uc state)
{
	static us secs = 0;
	switch(state)
	{
		case TS_Wait:
			state = start ? TS_Timer : TS_Wait;
			break;
		case TS_Timer:
			secs++;

			PORTB = (secs << 4);	
			
			state = game_over ? TS_Stop : TS_Timer;
			break;
		case TS_Stop:
			PORTB = secs % 5;
			state = TS_Stop;
			break;
		default: state = TS_Wait; break;
	}
	return state;
}

//******************************************************************** GAMEPLAY TICK

/*
enum GamePlayStates {GP_Init, GP_Wait, GP_Start_Btn_Pressed, GP_Drop, GP_Collision, GP_Game_Over};
uc GamePlayTick(uc state)
{
	switch(state)
	{
		case GP_Init:
			block_type = (rand() % 8);
			
			Init_LED_Matrices(4);
			Set_Brightness(0, 4);
			Clear_All();
			
			state = GP_Wait;
			break;
		case GP_Wait:
			state = start ? GP_Drop : GP_Wait;
			break;
		case GP_Drop:
			if (!collision && !Game_Over())
			{
				Clear_All();
				Draw_Block_Matrix();
				Drop_A_Block(
					&curr_matrix,
					&collision,
					&blocks[block_type],
					&from_bottom
				);
				
				state = GP_Drop;
			}
			else
			{
				if (collision && !Game_Over())
					state = GP_Collision;
				else
					state = GP_Game_Over;
			}
			break;
		case GP_Collision:
			block_type = (rand() % 8);
			collision = 0;
			curr_matrix = 0;
			from_bottom = 0;
			
			blocks[block_type].left  = blocks[block_type].init_left;
			blocks[block_type].right = blocks[block_type].init_right;
			blocks[block_type].orientation = Vertical_Up;
			
			Solid_Row_Eliminator();
			Draw_Block_Matrix();
			
			state = GP_Drop;
			break;
		case GP_Game_Over:
			Clear_All();
			
			state = GP_Wait;
			break;
		default: state = GP_Init; break;
	}
	return state;
}
*/
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
#endif
