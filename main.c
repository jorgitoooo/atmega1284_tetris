#include <avr/io.h>
//#include <stdlib.h>

#include "timer.h"
#include "io.c"
#include "MAX7219.c"

#define LED_MATRIX_PORT PORTB

//ADDRESSES FOR MAX7219
#define NO_OP      0x0000
#define DIG_0      0x0100
#define DIG_1      0x0200
#define DIG_2      0x0300
#define DIG_3      0x0400
#define DIG_4      0x0500
#define DIG_5      0x0600
#define DIG_6      0x0700
#define DIG_7      0x0800
#define DECODE_MODE_REG_ADDR 0x0900
#define INTENSITY_REG_ADDR   0x0A00
#define SCAN_LIMIT_REG_ADDR  0x0B00
#define SHUTDOWN_REG_ADDR    0x0C00
#define DISPLAY_TST_REG_ADDR 0x0F00

//DECODE MODES
#define NO_DECODE  0x00
#define ALL_DIGITS 0xFF
//INTENSITIES
#define INTENSITY_LOW  0x00
#define INTENSITY_MID  0x07
#define INTENSITY_HIGH 0x0F

//SCAN LIMITS
#define SCAN_LOW_NIB 0x03
#define SCAN_BYTE    0x07

//SHUTDOWN MODES
#define SHUTDOWN_MODE        0x00
#define SHUTDOWN_NORMAL_MODE 0x01

//DISPLAY TEST MODES
#define DISP_NORMAL_MODE 0x00
#define DISP_TEST_MODE   0x01

#define MAX_CLK ((~PINA >> 3) & 0X01)
#define MAX_CLK_HIGH 0x01
#define MAX_CLK_LOW  0x00
#define MAX_CLK_BIT 3

#define LOAD  0x02
#define CLEAR 0x00

#define BOTTOM_MATRIX 3

typedef unsigned char  uc;
typedef signed char    sc;
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

void Load_Word()
{
	LOAD_1();
	LOAD_0();
}

void Draw_O_Block(char matrix, uc left, uc right, sc from_bottom)
{
	uc blk;
	if (matrix != BOTTOM_MATRIX && from_bottom == -1)
	{
		blk = 0x01; // Half of the block is on the lower matrix
	}
	else if (from_bottom == 7)
	{ 
		blk = 0x01 << from_bottom; // Half of the block is on the upper matrix
	}
	else
	{
		blk = 0x03 << from_bottom; // Regular block
	}
	
	for (char j = right; j <= left; j++)
	{
		for(char k = 0; k < 4; k++)
		{
			if (k == matrix)
			{	
				MAX7219_SendByte(j);
				MAX7219_SendByte(blk);
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

enum Block_Orientation { Vertical_Up, Horizontal_Up, Vertical_Down, Horizontal_Down} Block_Orientation;

uc Assign_L_Block(uc orientation, uc h_pos)
{
	uc L = 0x00;
	switch(orientation)
	{
		case Vertical_Up:
			L = h_pos == 0 ? 0x01 : 0x07;
			break;
		case Horizontal_Up:
			L = h_pos == 0 ? 0x03 : 0x01;
			break;
		case Vertical_Down:
			L = h_pos == 0 ? 0x07 : 0x04;
			break;
		case Horizontal_Down:
			L = h_pos == 2 ? 0x03 : 0x02;
			break;
		default: break;
	}
	return L;
}

void Draw_L_Block(char matrix, uc left, uc right, sc from_bottom, uc orientation)
{
	for(char pos = right; pos <= left; pos++)
	{
		for(char k = 0; k < 4; k++)
		{
			if (k == matrix)
			{
				uc l_blk = Assign_L_Block(orientation, pos - right);
				l_blk = from_bottom < 0 ? l_blk >> abs(from_bottom) : l_blk << from_bottom;
				
				MAX7219_SendByte(pos);
				MAX7219_SendByte(l_blk);
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

uc Assign_I_Block(uc orientation)
{
	uc bar = 0x00;
	switch(orientation)
	{
		case Vertical_Up:
		case Vertical_Down:
			bar = 0x0F;
			break;
		case Horizontal_Down:
		case Horizontal_Up:
			bar = 0x01;
			break;
		default: break;
	}
	return bar;
}

void Draw_I_Block(char matrix, uc left, uc right, sc from_bottom, uc orientation)
{
/*
	uc elle;
	if (matrix != BOTTOM_MATRIX)
	{
		if(from_bottom == -1)
		{
			
		}
	}
	else if (from_bottom == 7)
	{
		elle = 0x01 << from_bottom; // Half of the block is on the upper matrix
	}
	else
	{
		elle = 0x03 << from_bottom; // Regular block
	}
*/
	for(char pos = right; pos <= left; pos++)
	{
		for(char k = 0; k < 4; k++)
		{
			if (k == matrix)
			{
				MAX7219_SendByte(pos);
				MAX7219_SendByte( Assign_I_Block(orientation) << from_bottom );
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

	TimerSet(250);
	TimerOn();

	Init_LED_Matrices(4);
	Set_Brightness(2, 4);
	Clear_All();

	char cur_matrix = 0;
	signed char j = 7;
	char is_done = 0x00;
	
	uc right = 1, left = 2;
	
	while (1) 
    {

//		Draw_L_Block(3, 5, 4, 0, Vertical_Down);

		if (!is_done)
		{
			PORTB |= 0x40;
			
			if (~PINA & 0x08 && left < 8) // Shifts entire block to the left
			{
				++left;
				++right;
			}
			else if (~PINA & 0x08 && left == 8) // resets block on the rhs
			{
				left = 2;
				right = 1;
			}
		
			Clear_All();
/*
			if (j < 0 && cur_matrix < BOTTOM_MATRIX) // L-BLOCK
			{
//				Draw_L_Block(cur_matrix,     left, right,     j, Vertical_Up);
//				Draw_L_Block(cur_matrix + 1, left, right, 8 + j, Vertical_Up);
			}

			--j;
*/
			// j < 0 if part of the block is on the lower matrix
			if (j < 0 && cur_matrix < BOTTOM_MATRIX) // O-BLOCK
			{
				Draw_O_Block(cur_matrix    , left, right, j);
				Draw_O_Block(cur_matrix + 1, left, right, 8 + j);
//				Draw_L_Block(cur_matrix,     left, right,     j, Vertical_Up);
//				Draw_L_Block(cur_matrix + 1, left, right, 8 + j, Vertical_Up);
			}
			else Draw_O_Block(cur_matrix, left, right, j);
//			else Draw_L_Block(cur_matrix, left, right, j, Vertical_Up);
			--j;
						
			while(!TimerFlag);
			TimerFlag = 0;

	
			if(j < -1 && cur_matrix < 3) // O-BLOCK
//			if(j < -2 && cur_matrix < 3) // L-BLOCK
			{
				j = 6; // O-BLOCK
//				j = 5; // L-BLOCK
				++cur_matrix;
			}
			else if (j < 0 && cur_matrix >= 3) is_done = 0x01;
		}

		/*
		if (~PINA & 0x04) 
		{
			is_done = 0x00;
			cur_matrix = 0;
			j = 7;
		}
		*/

    }
}

