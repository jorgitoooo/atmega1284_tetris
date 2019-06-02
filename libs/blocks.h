#ifndef __BLOCKS_H_K__
#define __BLOCKS_H_K__

typedef unsigned char uc;
typedef signed char   sc;

enum Block_Orientation { Vertical_Up, Horizontal_Up, Vertical_Down, Horizontal_Down};

void Draw_O_Block(char matrix, uc left, uc right, sc from_bottom);

uc Assign_L_Block(uc orientation, uc h_pos);
void Draw_L_Block(char matrix, uc left, uc right, sc from_bottom, uc orientation);


uc Assign_I_Block(uc orientation);
void Draw_I_Block(char matrix, uc left, uc right, sc from_bottom, uc orientation);

#endif
