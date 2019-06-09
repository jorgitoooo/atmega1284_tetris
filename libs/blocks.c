
#ifndef __BLOCKS_C__
#define __BLOCKS_C__

#include "blocks.h"
#include "utils.h"

// GLOBALS
//*******************************
uc POS_ARRAY[4][8];

#define NUM_OF_BLOCKS 7
const uc num_of_blocks = 7;

typedef struct Block
{
	uc left;
	uc right;
	uc init_left;
	uc init_right;
	uc orientation;
	uc width;
	uc rotation_change; // To adjust the blocks width
	uc (*fct) (uc, uc, uc, sc, uc, uc);
} Block;
//*******************************
void Initialize_Blocks(Block blocks[])
{
	uc block = 0;
	
	// L-Block
	blocks[block].init_left       = 5;
	blocks[block].init_right      = 4;
	blocks[block].left            = blocks[block].init_left;
	blocks[block].right           = blocks[block].init_right;
	blocks[block].width           = blocks[block].left - blocks[block].right;
	blocks[block].rotation_change = 1;
	blocks[block].fct = &Draw_L_Block;
	block++;
	// O-Block
	blocks[block].init_left	      = 5;
	blocks[block].init_right      = 4;
	blocks[block].left	      = blocks[block].init_left;
	blocks[block].right	      = blocks[block].init_right;
	blocks[block].width           = blocks[block].left - blocks[block].right;
	blocks[block].rotation_change = 0;
	blocks[block].fct = &Draw_O_Block;
	block++;
	// I-Block
	blocks[block].init_left	      = 5;
	blocks[block].init_right      = 5;
	blocks[block].left	      = blocks[block].init_left;
	blocks[block].right	      = blocks[block].init_right;
	blocks[block].width           = blocks[block].left - blocks[block].right;
	blocks[block].rotation_change = 2;
	blocks[block].fct = &Draw_I_Block;
	block++;
	// J-Block
	blocks[block].init_left	      = 5;
	blocks[block].init_right      = 4;
	blocks[block].left	      = blocks[block].init_left;
	blocks[block].right	      = blocks[block].init_right;
	blocks[block].width           = blocks[block].left - blocks[block].right;
	blocks[block].rotation_change = 1;
	blocks[block].fct = &Draw_J_Block;
	block++;
	// S-Block
	blocks[block].init_left	      = 5;
	blocks[block].init_right      = 4;
	blocks[block].left	      = blocks[block].init_left;
	blocks[block].right	      = blocks[block].init_right;
	blocks[block].width           = blocks[block].left - blocks[block].right;
	blocks[block].rotation_change = 1;
	blocks[block].fct = &Draw_S_Block;
	block++;
	// Z-Block
	blocks[block].init_left	      = 5;
	blocks[block].init_right      = 4;
	blocks[block].left	      = blocks[block].init_left;
	blocks[block].right	      = blocks[block].init_right;
	blocks[block].width           = blocks[block].left - blocks[block].right;
	blocks[block].rotation_change = 1;
	blocks[block].fct = &Draw_Z_Block;
	block++;
	// T-Block
	blocks[block].init_left       = 5;
	blocks[block].init_right      = 4;
	blocks[block].left            = blocks[block].init_left;
	blocks[block].right           = blocks[block].init_right;
	blocks[block].width           = blocks[block].left - blocks[block].right;
	blocks[block].rotation_change = 1;
	blocks[block].fct = &Draw_T_Block;
	block++;
}


//****************************************** O - Block
uc Assign_O_Block(uc orientation, uc h_pos)
{
	return 0x03;
}

uc Draw_O_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact)
{
	return Draw_It(matrix, left, right, from_bottom, orientation, made_contact, &Assign_O_Block);
}
//****************************************** L - Block
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

uc Draw_L_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact)
{
	return Draw_It(matrix, left, right, from_bottom, orientation, made_contact, &Assign_L_Block);
}
//****************************************** I - Block
uc Assign_I_Block(uc orientation, uc h_pos)
{
	uc bar = 0x00;
	switch(orientation)
	{
		case Vertical_Up:
		case Vertical_Down:
			bar = 0x07;
			break;
		case Horizontal_Down:
		case Horizontal_Up:
			bar = 0x01;
			break;
		default: break;
	}
	return bar;
}

uc Draw_I_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact)
{
	return Draw_It(matrix, left, right, from_bottom, orientation, made_contact, &Assign_I_Block);
}
//****************************************** J - Block
uc Assign_J_Block(uc orientation, uc h_pos)
{
	uc J = 0x00;
	switch(orientation)
	{
		case Vertical_Up:
			J = h_pos == 0 ? 0x07 : 0x01;
			break;
		case Horizontal_Up:
			J = h_pos == 2 ? 0x03 : 0x01;
			break;
		case Vertical_Down:
			J = h_pos == 0 ? 0x04 : 0x07;
			break;
		case Horizontal_Down:
			J = h_pos == 0 ? 0x03 : 0x02;
			break;
		default: break;
	}
	return J;
}

uc Draw_J_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact)
{
	return Draw_It(matrix, left, right, from_bottom, orientation, made_contact, &Assign_J_Block);
}
//****************************************** S - Block
uc Assign_S_Block(uc orientation, uc h_pos)
{
	uc S = 0x00;
	switch(orientation)
	{
		case Vertical_Up:
		case Vertical_Down:
			S = h_pos == 0 ? 0x03 : 0x06;
			break;
		case Horizontal_Up:
		case Horizontal_Down:
			if      (h_pos == 0) S =  0x02;
			else if (h_pos == 1) S = 0x03;
			else S = 0x01;
			break;
		default: break;
	}
	return S;
}

uc Draw_S_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact)
{
	return Draw_It(matrix, left, right, from_bottom, orientation, made_contact, &Assign_S_Block);
}
//****************************************** Z - Block
uc Assign_Z_Block(uc orientation, uc h_pos)
{
	uc Z = 0x00;
	switch(orientation)
	{
		case Vertical_Up:
		case Vertical_Down:
			Z = h_pos == 0 ? 0x06 : 0x03;
			break;
		case Horizontal_Up:
		case Horizontal_Down:
			if      (h_pos == 0) Z = 0x01;
			else if (h_pos == 1) Z = 0x03;
			else Z = 0x02;
			break;
/*		case Vertical_Down:
			Z = h_pos == 0 ? 0x06 : 0x03;
			break;
		case Horizontal_Down:
			if      (h_pos == 0) Z = 0x01;
			else if (h_pos == 1) Z = 0x03;
			else Z = 0x02;
			break;
*/		default: break;
	}
	return Z;
}

uc Draw_Z_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact)
{
	return Draw_It(matrix, left, right, from_bottom, orientation, made_contact, &Assign_Z_Block);
}
//****************************************** T - Block
uc Assign_T_Block(uc orientation, uc h_pos)
{
	uc T = 0x00;
	switch(orientation)
	{
		case Vertical_Up:
			T = h_pos == 0 ? 0x02 : 0x07;
			break;
		case Vertical_Down:
			T = h_pos == 0 ? 0x07 : 0x02;
			break;
		case Horizontal_Up:
			T = h_pos == 1 ? 0x03 : 0x01;
			break;
		case Horizontal_Down:
			T = h_pos == 1 ? 0x03 : 0x02;
			break;
		default: break;
	}
	return T;
}

uc Draw_T_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact)
{
	return Draw_It(matrix, left, right, from_bottom, orientation, made_contact, &Assign_T_Block);
}

uc Draw_It(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact, uc (*Assign_Block) (uc, uc))
{
	uc block_contact = made_contact;
	
	uc block_length = left - right + 1; // Lowest (left - right) value is 0 which is why we add 1
	uc block_maintainer[block_length]; // Will maintain the visual representation of the block

	for(char pos = 1; pos <= 8; pos++)
	{
		for(char curr_matrix = 0; curr_matrix < 4; curr_matrix++)
		{
			if (curr_matrix == matrix)
			{
				uc l_blk = (*Assign_Block)(orientation, pos - right);

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

	// Update block POS_ARRAY on contact
	if (block_contact)
		for (uc block_seg_pos = 0 ; block_seg_pos < block_length; block_seg_pos++)
			POS_ARRAY[matrix][right - 1 + block_seg_pos] |= block_maintainer[block_seg_pos];
	return block_contact;
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

uc Check_For_Contact(uc matrix, uc pos, sc poc, uc block)
{
	// ************** HORIZONTAL_DOWN LOWER MATRIX COLLISION BUG IS PROB. HERE **
	// Neg. poc means part of the block is on the lower matrix
	// Matrix + 1 > 3 means that we're on the lowest matrix
	if (poc <= 0 && matrix + 1 <= 3)
	{
		return ((POS_ARRAY[matrix + 1][pos] & (block << (7 + poc))) > 0);
	}
	return ((POS_ARRAY[matrix][pos] & (block >> 1)) > 0) || (poc == 0 && matrix == 3);
}

uc Game_Over()
{
	for (uc col = 0; col < 8; col++)
		if ((POS_ARRAY[0][col] & 0x80) > 0)
			return 1;
}
#endif
