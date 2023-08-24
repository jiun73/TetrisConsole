#pragma once
#include "TetrisGlobal.h"
#include <vector>

class Tetromino
{

	std::vector<TetrominoBits> tetrominos =
	{
		(TetrominoBits)0b0100010001000100,
		(TetrominoBits)0b1100011000000000,
		(TetrominoBits)0b1000100011000000,
		(TetrominoBits)0b1100110000000000,
		(TetrominoBits)0b0100111000000000,
		(TetrominoBits)0b0110110000000000,
	};


private:
	int type = 0;

public:
	V2d_i pos;
	TetrominoBits data = tetrominos.at(0);

	int getType() 
	{
		return type;
	}

	void setType(int i) 
	{
		type = i;
		data = tetrominos.at(i);
	}

	void draw(ConsoleRenderer& ren, const V2d_i& pos)
	{
		int16_t bits = tetrominos.at(type);
		for (int i = 0; i < 16; i++)
		{
			int16_t mask = (1 << (16 - i));
			bool current = (bits & mask) >> (16 - i);
			if (current)
			{
				V2d_i r = { 4 - (i % 4), i / 4 };
				ren.drawPixel(r + pos);
			}
		}
	}

};