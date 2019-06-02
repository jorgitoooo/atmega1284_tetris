#include <avr/io.h>
#include <stdlib.h>
#include <string.h>

#include "timer.h"
#include "io.c"
#include "MAX7219.c"

#include "consts.h"
#include "utils.c" // Init_LED_Matrices, Set_Brightness, Load_word, ...
#include "blocks.c" // Block drawing funcs


typedef unsigned  char uc;
typedef   signed  char sc;
typedef unsigned short us;



// *** WILL RETURN IF BLOCK REACHES THE BOTTOM OR MAKES CONTACT WITH A RESTING BLOCK ***
void Drop_A_Block(uc (*fct) (uc, uc, uc, sc, uc), uc *cur_matrix, uc *is_done, uc *left, uc *right, uc *orientation, sc *from_bottom)
{
	static uc contact = 0;
	//uc contact = 0; // Is one if block makes contact with resting block
	//static uc pos_array[4][8]; // Maintains value of dropped blocks
	
	if (!(*is_done))
	{
//		PORTB |= 0x40; // test light
		
		// *** MAY ABSTRACT AWAY ***
		if (~PINA & 0x08 && *left < 8) // Shift check
		{
			// When horizontal, there should be an extra space left for the left/right most light
			++(*left);
			++(*right);
		}
		else if (~PINA & 0x08 && *left == 8) // resets block on the rhs
		{
			if (*orientation == Horizontal_Down || *orientation == Horizontal_Up)
				*left = 3;
			else
				*left = 2;
			*right = 1;
		}

		// *** MAY ABSTRACT AWAY ***
		if (~PINA & 0x04) // Orientation check
		{
			*orientation = (*orientation + 1) % 4;
			
			if ((*orientation == Horizontal_Down || *orientation == Horizontal_Up) && (*left - *right) == 1)
			{
				// Maintains left side in bound
				if (*left > 7)
				--(*right);
				else
				++(*left);
			}
			else if ((*left - *right) == 2)
			--(*left);
		}
		
		Clear_All();

		// from_bottom < 0 if part of the block is on the lower matrix
		if (*from_bottom < 0 && *cur_matrix < BOTTOM_MATRIX)
		{
			// *** MAY BE A SPECIAL CASE FOR CONTACT ***
			contact = (*fct)(*cur_matrix + 1, *left, *right, 8 + *from_bottom, *orientation); // Not placing this fct call first erases the upper matrix's part of block
			contact = (*fct)(*cur_matrix,     *left, *right,     *from_bottom, *orientation);
			
			// Test and assign lower matrix pos_array	
			// - Assign upper matrix pos_array if lower matrix is assigned
		}
		else
		{
			// Draw_* funcs may need to return the value of block and its pos ???
			contact = (*fct)(*cur_matrix, *left, *right, *from_bottom, *orientation);
			// Test and assign current matrix pos_array
		//	contact = Check_For_Contact(*cur_matrix, *from_bottom, block_value);			
		}
		--(*from_bottom);
		
//		while(!TimerFlag);
//		TimerFlag = 0;
		
		if(*from_bottom < -2 && *cur_matrix < 3 && !contact)
		{
			*from_bottom = 5;
			++(*cur_matrix);
		}
		else if (*from_bottom < 0 && *cur_matrix >= 3 || contact)
		{
			*is_done = 0x01;
			// assign position of block to pos_array
		}
	}
}

void MakeRandom()
{
	uc matrix = 0; uc col = 0;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	matrix++; col = 0;
	POS_ARRAY[matrix][col++] = 0b00111111;
	POS_ARRAY[matrix][col++] = 0b00011111;
	POS_ARRAY[matrix][col++] = 0b00011111;
	POS_ARRAY[matrix][col++] = 0b00001111;
	POS_ARRAY[matrix][col++] = 0b00001111;
	POS_ARRAY[matrix][col++] = 0b00111111;
	POS_ARRAY[matrix][col++] = 0b00001111;
	POS_ARRAY[matrix][col++] = 0b00000011;	
	matrix++; col = 0;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	matrix++; col = 0;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
}

void MakeWall()
{
	uc matrix = 0; uc col = 0;
/*
	POS_ARRAY[matrix][col++] = 0b00000001;
	POS_ARRAY[matrix][col++] = 0b00000001;
	POS_ARRAY[matrix][col++] = 0b00000001;
	POS_ARRAY[matrix][col++] = 0b00000001;
	POS_ARRAY[matrix][col++] = 0b00000001;
	POS_ARRAY[matrix][col++] = 0b00000001;
	POS_ARRAY[matrix][col++] = 0b00000001;
	POS_ARRAY[matrix][col++] = 0b00000001;
*/
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	POS_ARRAY[matrix][col++] = 0b00000000;
	matrix++; col = 0;
	POS_ARRAY[matrix][col++] = 0b00000011;
	POS_ARRAY[matrix][col++] = 0b00000011;
	POS_ARRAY[matrix][col++] = 0b00000011;
	POS_ARRAY[matrix][col++] = 0b00000011;
	POS_ARRAY[matrix][col++] = 0b00000011;
	POS_ARRAY[matrix][col++] = 0b00000011;
	POS_ARRAY[matrix][col++] = 0b00000011;
	POS_ARRAY[matrix][col++] = 0b00000011;
	matrix++; col = 0;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	matrix++; col = 0;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
}
void DrawPosMatrix()
{
	for (uc column = 0; column < 8; column++)
	{
		for (uc matrix = 0; matrix < 4; matrix++)
		{
			MAX7219_SendByte(column + 1);
			MAX7219_SendByte(POS_ARRAY[matrix][column]);
			Load_Word();
			
			while(!TimerFlag);
			TimerFlag = 0;
		}
	}
}

int main(void)
{
	DDRA = 0x03; PORTA = 0xFC;
//	DDRD = 0xFF; PORTD = 0x00;
	
	DDRB = 0xFF; PORTB = 0x00;

	TimerSet(450);
	TimerOn();

	Init_LED_Matrices(4);
	Set_Brightness(0, 4);
	Clear_All();

	uc cur_matrix = 0;
	sc from_bottom = 7;
	uc is_done = 0;
	
	uc right = 4, left = 5;
	
	enum Block_Orientation orientation = Vertical_Up;
	
	uc (*fct) (uc, uc, uc, sc, uc);
	
	fct = &Draw_L_Block;

	MakeRandom();
//	DrawPosMatrix();
	
	while (1) 
    {
//		Show_Orientations("l block");	
		Drop_A_Block(fct, &cur_matrix, &is_done, &left, &right, &orientation, &from_bottom);
	
//		PORTB |= 0b01101111 & ( 0x01 << 3);
		while(!TimerFlag);
		TimerFlag = 0;
    }
}

/*
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
*/