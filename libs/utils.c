#ifndef __UTILS_H__
#define __UTILS_H__

#include "blocks.c"

typedef unsigned  char uc;
typedef   signed  char sc;
typedef unsigned short us;

void Load_Word()
{
	LOAD_1();
	LOAD_0();
}

void Shift_Bit_Into(us reg_data, uc indx)
{
	// Move reg bit at position indx into DIN
	LED_MATRIX_PORT |= (reg_data >> indx) & 0x01; // 0000 00xc
	
	// CLK goes high to shift the bit
	LED_MATRIX_PORT |= 0x04; // 0000 0cxx
}
/*
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
*/
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

void Send_Word(uc reg, uc data)
{
	MAX7219_SendByte(reg);
	MAX7219_SendByte(data);
}

void Clear_All()
{
	for (uc reg = 1; reg < 9; reg++)
	{
		for (uc matrix = 0; matrix < 4; matrix++)
		{
			Send_Word(reg, 0x00);
		}
		Load_Word();
	}
}

void Set_Brightness(uc brightness, uc num_of_matrices)
{
	for (char i = 0; i < num_of_matrices; i++)
	{
		MAX7219_SetBrightness(brightness);
	}
}

void Init_LED_Matrices(uc num_of_matrices)
{
	for (char i = 0; i < num_of_matrices; i++)
	{
		MAX7219_Init();
	}
}


void Show_Orientations(const char *str)
{
	if(strcmp(str, "l block") == 0)
	{
		Draw_L_Block(0, 5, 4, 2, Vertical_Up, 0);
		Draw_L_Block(1, 5, 4, 2, Vertical_Down, 0);
		Draw_L_Block(2, 5, 3, 3, Horizontal_Down, 0);
		Draw_L_Block(3, 5, 3, 3, Horizontal_Up, 0);
	}
	else if(strcmp(str, "o block") == 0)
	{
		Draw_O_Block(0, 5, 4, 3, 0);
		Draw_O_Block(1, 5, 4, 3, 0); 
		Draw_O_Block(2, 5, 4, 3, 0);
		Draw_O_Block(3, 5, 4, 3, 0);
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
#endif
