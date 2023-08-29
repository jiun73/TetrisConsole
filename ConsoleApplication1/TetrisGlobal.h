#pragma once
#include "ConsoleRenderer.h"

typedef uint16_t TetrominoBits;

//returns a mask to use on tetrominos to get a given line
uint16_t lineMask(int line);

//returns a given tetromino line
uint16_t getTetrominoLine(uint16_t tetromino, int line);