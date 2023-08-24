#include "TetrisGlobal.h"

//returns a mask to use on tetrominos to get a given line
uint16_t lineMask(int line)
{
	uint16_t a = 0b1111000000000000;
	a >>= (line * 4);
	a ^= (a << 4);
	return a;
}

uint16_t getTetrominoLine(uint16_t tetromino, int line)
{
	uint16_t a = lineMask(line);
	return ((a & tetromino) >> ((3 - line) * 4) & 0b1111);
}