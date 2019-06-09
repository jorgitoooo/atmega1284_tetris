#include "utils.h"
#include "blocks.h"

void Load_Word()
{
	LOAD_1();
	LOAD_0();
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
	CLK_0();
}

void Init_LED_Matrices(uc num_of_matrices)
{
	for (char i = 0; i < num_of_matrices; i++)
	{
		MAX7219_Init();
	}
	CLK_0();
}
