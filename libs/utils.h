#ifndef __UTILS_H__
#define __UTILS_H__

#include "MAX7219.h"
#include "blocks.c"

typedef unsigned  char uc;
typedef   signed  char sc;
typedef unsigned short us;

void Load_Word();

void Send_Word(uc reg, uc data);

void Clear_All();

void Set_Brightness(uc brightness, uc num_of_matrices);

void Init_LED_Matrices(uc num_of_matrices);

#endif
