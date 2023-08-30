#pragma once
#include "TetrisGlobal.h"
#include <vector>
#include <array>

extern std::array<TetrominoBits, 4 * 7> tetrominos;

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

private:
	int type = 0;
	int rotation = 0;

public:
	V2d_i pos;
	TetrominoBits data = tetrominos.at(0);

	TetrominoBits getData();

	int getRotation() { return rotation; }

	void setRotation(int r) 
	{
		rotation = r;
		data = getData();

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
		p.glyph = (char)219;

		uint16_t bits = getData();

		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				uint16_t line = getTetrominoLine(bits, y);
				uint16_t current = line & (1 << x);
				if (current)
					list.emplace(V2d_i(x, y) + pos, p);
			}
		}

		return list;
	}

	void draw(ConsoleRenderer& ren, const V2d_i& pos)
	{
		ren.setDrawColor(colors[type], BG_BLACK);
		uint16_t bits = getData();

		for (int y = 0; y < 4; y++)
		{
			for (int x = 0; x < 4; x++)
			{
				uint16_t line = getTetrominoLine(bits, y);
				uint16_t current = line & (1 << x);
				if(current)
					ren.drawPixel(V2d_i(x,y) + pos);
			}
		}
	}

};