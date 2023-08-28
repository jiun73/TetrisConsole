#pragma once
#include "ConsoleRenderer.h"

typedef uint16_t TetrominoBits;

//void drawTetromino(ConsoleRenderer& ren, const V2d_i& pos, const size_t& id);
//returns a mask to use on tetrominos to get a given line
uint16_t lineMask(int line);

uint16_t getTetrominoLine(uint16_t tetromino, int line);