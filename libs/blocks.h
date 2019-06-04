#ifndef __BLOCKS_H__
#define __BLOCKS_H__

#include "utils.h"

typedef unsigned char  uc;
typedef   signed char  sc;
typedef unsigned short us;

// GLOBALS
//*******************************
uc POS_ARRAY[4][8];
//*******************************

uc Check_For_Contact(uc matrix, uc pos, sc poc, uc block);

void Solid_Row_Detector(uc matrix, uc *re);

uc Draw_O_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact);

enum Block_Orientation { Vertical_Up, Horizontal_Up, Vertical_Down, Horizontal_Down};

uc Assign_L_Block(uc orientation, uc h_pos);

uc Draw_L_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact);

uc Assign_I_Block(uc orientation);

uc Draw_I_Block(uc matrix, uc left, uc right, sc from_bottom, uc orientation, uc made_contact);
#endif
