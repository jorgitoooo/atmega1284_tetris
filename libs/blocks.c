
#ifndef __BLOCKS_C__
#define __BLOCKS_C__

//#include "utils.h"

//typedef unsigned char  uc;
//typedef   signed char  sc;
//typedef unsigned short us;

#include "blocks.h"
#include "utils.h"

// GLOBALS
//*******************************
uc POS_ARRAY[4][8];
//*******************************

uc Check_For_Contact(uc matrix, uc pos, sc poc, uc block)
{
	// ************** HORIZONTAL_DOWN LOWER MATRIX COLLISION BUG IS PROB. HERE **
	// Neg. poc means part of the block is on the lower matrix
	// Matrix + 1 > 3 means that we're on the lowest matrix
	if (poc <= 0 && matrix + 1 <= 3)
	{
		return ((POS_ARRAY[matrix + 1][pos] & (block << (7 + poc))) > 0);
	}

//	uc par_pos = (POS_ARRAY[matrix][pos] & (0x01 << (poc - 1))) >> (poc - 1);
//	uc blk_pos = (block & (0x01 << poc)) >> poc;

	// Returns 1 if block collides w/ another block or reaches the bottom
//	return (par_pos == blk_pos && par_pos != 0) || (poc == 0 && matrix == 3);

	return ((POS_ARRAY[matrix][pos] & (block >> 1)) > 0) || (poc == 0 && matrix == 3);
}

void Solid_Row_Detector(uc matrix, uc *re)
{
	// re = Rows to be Eliminated (aka solid rows)
	re[0] = 0; // Reset Value in case previously set
	
	uc is_solid = 1;
	uc index = 0;
	
	for (uc row = 0; row < 8; row++)
	{
		for (uc col = 0; col < 8; col++)
		{
			if ((POS_ARRAY[matrix][col] & (0x01 << row)) == 0)
			{
				col = 9;
				is_solid = 0;
			}
		}
		if (is_solid)
		{
			re[0]++;
			re[++index] = row;
		}
		is_solid = 1;
	}
}

void Draw_O_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation)
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

//enum Block_Orientation { Vertical_Up, Horizontal_Up, Vertical_Down, Horizontal_Down};

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

//uc Draw_L_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation)
uc Draw_L_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact)
{
	uc block_contact = made_contact;
	
	uc block_length = left - right + 1; // Lowest (left - right) value is 0 which is why we add 1
	uc block_maintainer[block_length]; // Will maintain the visual representation of the block

//	if (!block_contact)// If made_contact == 1 -> lower matrix segment of block made contact
	{
		for(char pos = 1; pos <= 8; pos++)
		{
			for(char curr_matrix = 0; curr_matrix < 4; curr_matrix++)
			{
				if (curr_matrix == matrix)
				{
					uc l_blk = Assign_L_Block(orientation, pos - right);

					l_blk = from_bottom < 0 ? l_blk >> abs(from_bottom) : l_blk << from_bottom;
				
					// right <= pos <= left -> block segment column is selected
					if (pos >= right && pos <= left)
					{
						if (Check_For_Contact(curr_matrix, pos - 1, from_bottom, l_blk) == 1)
						{
							block_contact = 1;
						}
						
						block_maintainer[pos - right] = l_blk;
						
						Send_Word(pos, POS_ARRAY[curr_matrix][pos-1] | l_blk);
					}
					else
					{
						Send_Word(pos, POS_ARRAY[curr_matrix][pos-1]);
					}
				}
				else
				{
					if (pos >= right && pos <= left && from_bottom < 0)
					{
						Send_Word(0, 0x00);
					}
					else
					{

						Send_Word(pos, POS_ARRAY[curr_matrix][pos-1]);
					}
				}
			}
			Load_Word();
		}
	}

	// Update block POS_ARRAY on contact
	if (block_contact)
	{
		for (uc block_seg_pos = 0 ; block_seg_pos < block_length; block_seg_pos++)
		{
			POS_ARRAY[matrix][right - 1 + block_seg_pos] |= block_maintainer[block_seg_pos];
		}
	}
	return block_contact;
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

void Draw_I_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation)
{
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
#endif
