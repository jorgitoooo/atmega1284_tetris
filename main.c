#include <avr/io.h>
#include <stdlib.h>
#include <string.h>

#include "timer.h"
#include "io.c"
#include "MAX7219.c"

void Load_Word()
{
	LOAD_1();
	LOAD_0();
}

#include "consts.h"
#include "blocks.c"

typedef unsigned  char uc;
typedef   signed  char sc;
typedef unsigned short us;

/*
 */
/// Practice
#define PA(pos) ((~PINA >> pos) & 0x01)
/*
 */

void Shift_Bit_Into(us reg_data, uc indx)
{
	// Move reg bit at position indx into DIN
	LED_MATRIX_PORT |= (reg_data >> indx) & 0x01; // 0000 00xc
	
	// CLK goes high to shift the bit
	LED_MATRIX_PORT |= 0x04; // 0000 0cxx
}

void Init_LED_Matrix(uc matrix_count)
{
	us us_disp_tst_reg = DISPLAY_TST_REG_ADDR | DISP_NORMAL_MODE;
	uc indx = 16;
	uc curr_matrix = 0;
	
	while (curr_matrix < matrix_count)
	{
		LED_MATRIX_PORT = CLEAR;
		
		if(indx > 0)
		{
			--indx;
		}
		else
		{
			// Latch 16-bit shift reg
			LED_MATRIX_PORT = LOAD; // 0000 0010
			indx = 15;
			++curr_matrix;
		}
		Shift_Bit_Into(us_disp_tst_reg, indx);
	}
}

void Set_Register(uc reg_data, uc matrix_num)
{
	LED_MATRIX_PORT = CLEAR;
	
	uc indx = 16;
	uc curr_matrix = 0;
	
	while (curr_matrix <= matrix_num)
	{
		LED_MATRIX_PORT = CLEAR;
		
		if(indx > 0)
		{
			--indx;
		}
		else
		{
			// Latch 16-bit shift reg
			if(curr_matrix == matrix_num) LED_MATRIX_PORT = LOAD; // 0000 0010
			
			indx = 15;
			++curr_matrix;

			while(!TimerFlag);
			TimerFlag = 0;
		}
		Shift_Bit_Into(reg_data, indx);
	}
	PORTB = 0x08;
}

//enum States {Start, Init, Pressed};

void Clear_All()
{
	for(char i = 0; i < 3; i++) MAX7219_Clear();
}

void Set_Brightness(uc brightness, uc num_of_matrices)
{
	for (char i = 1; i < num_of_matrices; i++)
	{
		MAX7219_SetBrightness(brightness);
	}
}

void Init_LED_Matrices(uc num_of_matrices)
{
	for (char i = 1; i < num_of_matrices; i++)
	{
		MAX7219_Init();
	}
}


void Show_Orientations(const char *str)
{
	if(strcmp(str, "l block") == 0)
	{
		Draw_L_Block(0, 5, 4, 2, Vertical_Up);
		Draw_L_Block(1, 5, 4, 2, Vertical_Down);
		Draw_L_Block(2, 5, 3, 3, Horizontal_Down);
		Draw_L_Block(3, 5, 3, 3, Horizontal_Up);
	}
	else if(strcmp(str, "o block") == 0)
	{
		Draw_O_Block(0, 5, 4, 3);
		Draw_O_Block(1, 5, 4, 3); 
		Draw_O_Block(2, 5, 4, 3);
		Draw_O_Block(3, 5, 4, 3);
	}
	else if(strcmp(str, "i block") == 0)
	{
		Draw_I_Block(0, 4, 4, 2, Vertical_Up);
		Draw_I_Block(1, 6, 2, 4, Horizontal_Up);
		Draw_I_Block(2, 5, 5, 2, Vertical_Down);
		Draw_I_Block(3, 7, 3, 3, Horizontal_Down);
	}
}

void Light_Matrix_Up(uc matrix)
{
	for (uc i = 0; i < 9; i++)
	{
		for (uc j = 0; j < 4; j++)
		{
			if (j == matrix)
			{
				MAX7219_SendByte(i);
				MAX7219_SendByte(0xFF);
			}
			else
			{
				MAX7219_SendByte(0);
				MAX7219_SendByte(0x00);
			}
		}
		Load_Word();
	}	
}

int main(void)
{
	DDRA = 0x03; PORTA = 0xFC;
	DDRD = 0xFF; PORTD = 0x00;
	
	DDRB = 0xFF; PORTB = 0x00;

	TimerSet(450);
	TimerOn();

	Init_LED_Matrices(4);
	Set_Brightness(2, 4);
	Clear_All();

	char cur_matrix = 0;
	signed char j = 7;
	char is_done = 0x00;
	
	uc right = 1, left = 2;
	
	enum Block_Orientation orientation = Vertical_Up;
	
	while (1) 
    {

//		Draw_L_Block(3, 5, 4, 0, Vertical_Down);

		if (!is_done)
		{
			PORTB |= 0x40;
		
			if (~PINA & 0x08 && left < 8) // Shifts entire block to the left
			{
				// When horizontal, there should be an extra space left for the left/right most light

				++left;
				++right;	
			}
			else if (~PINA & 0x08 && left == 8) // resets block on the rhs
			{
				if (orientation == Horizontal_Down || orientation == Horizontal_Up)
					left = 3;
				else
					left = 2;
				right = 1;
			}

		
			if (~PINA & 0x04)
			{
				orientation = (orientation + 1) % 4;
				
				if ((orientation == Horizontal_Down || orientation == Horizontal_Up) && (left - right) == 1)
				{
					// Maintains left in bound
					if (left > 7)
						--right;
					else
						++left;
				}
				else if ((left - right) == 2)
					--left;
				
			//	left = left > 8 ? 8 : left; // Keeps blocks in bound when horizontal
			}
		
			Clear_All();

			// j < 0 if part of the block is on the lower matrix
			if (j < 0 && cur_matrix < BOTTOM_MATRIX) // O-BLOCK
			{
//				Draw_O_Block(cur_matrix    , left, right, j);
//				Draw_O_Block(cur_matrix + 1, left, right, 8 + j);
				Draw_L_Block(cur_matrix,     left, right,     j, orientation);
				Draw_L_Block(cur_matrix + 1, left, right, 8 + j, orientation);
			}
//			else Draw_O_Block(cur_matrix, left, right, j);
			else Draw_L_Block(cur_matrix, left, right, j, orientation);
			--j;
						
			while(!TimerFlag);
			TimerFlag = 0;

	
//			if(j < -1 && cur_matrix < 3) // O-BLOCK
			if(j < -2 && cur_matrix < 3) // L-BLOCK
			{
//				j = 6; // O-BLOCK
				j = 5; // L-BLOCK
				++cur_matrix;
			}
			else if (j < 0 && cur_matrix >= 3) is_done = 0x01;
		}
    }
}

