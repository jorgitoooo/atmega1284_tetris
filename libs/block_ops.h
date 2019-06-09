#ifndef __BLOCK_OPS_H__
#define __BLOCK_OPS_H__

#include "blocks.h"

void Solid_Row_Eliminator();

void Collapsed_Row_Merger(uc re[4][9]);

void Solid_Row_Collapser(uc curr_matrix, uc row);

void Draw_Block_Matrix();

void Clear_Block_Matrix();

//************** WILL REMOVE
void MakeWall();
void MakeRandom();

#endif
