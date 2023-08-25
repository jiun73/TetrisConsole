#pragma once
#include "TetrisGlobal.h"
#include <vector>
#include <array>

class Tetromino
{
	std::vector<fgConsoleColors> colors =
	{
		CYAN,
		GREEN,
		BLUE,
		YELLOW,
		PURPLE,
		RED,
		DARK_YELLOW,
	};

	std::array<TetrominoBits, 4 * 7> tetrominos =
	{
			(TetrominoBits)0b0100010001000100,
			(TetrominoBits)0b1100011000000000,
			(TetrominoBits)0b1000100011000000,
			(TetrominoBits)0b1100110000000000,
			(TetrominoBits)0b0100111000000000,
			(TetrominoBits)0b0110110000000000,
			(TetrominoBits)0b0100010011000000,

			(TetrominoBits)0b0001111000000000,
			(TetrominoBits)0b0100110010000000,
			(TetrominoBits)0b1110100000000000,
			(TetrominoBits)0b1100110000000000,
			(TetrominoBits)0b0100011001000000,
			(TetrominoBits)0b1000110001000000,
			(TetrominoBits)0b1000111000000000,

			(TetrominoBits)0b0010001000100010,
			(TetrominoBits)0b1100011000000000,
			(TetrominoBits)0b0110001000100000,
			(TetrominoBits)0b1100110000000000,
			(TetrominoBits)0b0000111001000000,
			(TetrominoBits)0b0110110000000000,
			(TetrominoBits)0b1100100010000000,

			(TetrominoBits)0b0000000111100000,
			(TetrominoBits)0b0100110010000000,
			(TetrominoBits)0b0010111000000000,
			(TetrominoBits)0b1100110000000000,
			(TetrominoBits)0b0100110001000000,
			(TetrominoBits)0b1000110001000000,
			(TetrominoBits)0b1110001000000000,
	};

private:
	int type = 0;
	int rotation = 0;

public:
	V2d_i pos;
	TetrominoBits data = tetrominos.at(0);

	TetrominoBits getData() 
	{
		return tetrominos.at(type + (rotation * 7));
	}

	void rotate(bool left)
	{
		if (left)
			rotation--;
		else
			rotation++;

		if (rotation < 0)
			rotation = 3;
		if (rotation > 3)
			rotation = 0;

		data = getData();
	}

	int getType() 
	{
		return type;
	}

	void setType(int i) 
	{
		type = i;
		data = getData();
	}

	std::map<V2d_i, ConsolePixel> drawList() 
	{
		std::map<V2d_i, ConsolePixel> list;

		ConsolePixel p;
		p.fg = colors[type];
		p.bg = BG_BLACK;
		p.glyph = '@';

		int16_t bits = getData();
		for (int i = 0; i < 16; i++)
		{
			int16_t mask = (1 << (16 - i));
			bool current = (bits & mask) >> (16 - i);
			if (current)
			{
				V2d_i r = { 4 - (i % 4), i / 4 };
				list.emplace((r + pos), p);
			}
		}

		return list;
	}

	void draw(ConsoleRenderer& ren, const V2d_i& pos)
	{
		ren.setDrawColor(colors[type], BG_BLACK);
		int16_t bits = getData();
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