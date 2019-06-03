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
	POS_ARRAY[matrix][col++] = 0b01111111;
	POS_ARRAY[matrix][col++] = 0b00011111;
	POS_ARRAY[matrix][col++] = 0b00011111;
	POS_ARRAY[matrix][col++] = 0b00001111;
	POS_ARRAY[matrix][col++] = 0b00000111;
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
	POS_ARRAY[matrix][col++] = 0b11111101;
	matrix++; col = 0;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11111101;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11011111;
	POS_ARRAY[matrix][col++] = 0b11111111;
	POS_ARRAY[matrix][col++] = 0b11110111;
	POS_ARRAY[matrix][col++] = 0b11111111;
}

void MakeWall()
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

void Drop_A_Block(uc (*fct) (uc, uc, uc, sc, uc, uc), uc *cur_matrix, uc *is_done, uc *left, uc *right, uc *orientation, sc *from_bottom)
{
	uc contact = 0;
	
	if (!(*is_done))
	{	
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
			// Not placing this fct call first erases the upper matrix's part of block
			contact = (*fct)(*cur_matrix + 1, *left, *right, 8 + *from_bottom, *orientation, 0); 
		}
		contact = (*fct)(*cur_matrix, *left, *right, *from_bottom, *orientation, contact);
		--(*from_bottom);
		
		if(*from_bottom < -2 && *cur_matrix < 3 && !contact)
		{
			*from_bottom = 5;
			++(*cur_matrix);
		}
		else if (*from_bottom < 0 && *cur_matrix >= 3 || contact)
		{
			*is_done = 0x01;
		}
	}
}


void DrawBlockMatrix()
{
	for (uc col = 0; col < 8; col++)
	{
		for (uc matrix = 0; matrix < 4; matrix++)
		{
			MAX7219_SendByte(col+1);
			MAX7219_SendByte(POS_ARRAY[matrix][col]);
		}
		Load_Word();
	}
}

void TestForSolids()
{
	uc re[9];
	
	for (uc matrix = 0; matrix < 4; matrix++)
	{
		Check_For_Solid_Row(matrix, re);
		
		while (!TimerFlag);
		TimerFlag = 0;
		
		PORTB &= 0x0F;
		PORTB |= (re[0] << 4);
	}
}

int main(void)
{
	DDRA = 0x03; PORTA = 0xFC;
	DDRB = 0xFF; PORTB = 0x00;

	TimerSet(1000);
	TimerOn();

	Init_LED_Matrices(4);
	Set_Brightness(0, 4);
	Clear_All();

	uc cur_matrix = 0;
	sc from_bottom = 7;
	uc is_done = 0;
	
	uc cur_matrix2 = 0;
	sc from_bottom2 = 7;
	uc is_done2 = 0;
	
	uc right = 4, left = 5;
	uc right2 = 4, left2 = 5;
	
	enum Block_Orientation orientation = Vertical_Up;
	enum Block_Orientation orientation2 = Vertical_Up;
	
	uc (*fct) (uc, uc, uc, sc, uc, uc);
	
	fct = &Draw_L_Block;

	MakeRandom();
	DrawBlockMatrix();
	
	TestForSolids();	
	
//	uc re[9];
//	Check_For_Solid_Row(1, re);
	
//	PORTB = re[0];
	
	while (1) 
    {
	
//		Show_Orientations("l block");	
//		Drop_A_Block(fct, &cur_matrix, &is_done, &left, &right, &orientation, &from_bottom);
	
		if(is_done)
		{
			Clear_All();
			DrawBlockMatrix();
			Drop_A_Block(fct, &cur_matrix2, &is_done2, &left2, &right2, &orientation2, &from_bottom2);
		}

		while(!TimerFlag);
		TimerFlag = 0;
    }
}