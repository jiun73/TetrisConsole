#pragma once
#include "TetrisGlobal.h"
#include "Tetrominos.h"
#include <vector>
#include <bitset>

class TetrisBoard
{
private:
	//a list of 16-bit integers representing the Tetris playspace
	std::vector<uint16_t> board = {
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,

		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1000000000010000,
		(uint16_t)0b1111111111110000,
	};

	std::map<V2d_i, ConsolePixel> placedBlocks;


public:
	TetrisBoard() {}
	~TetrisBoard() {}

	//return if a tetromino is colliding with the board
	bool isColliding(Tetromino& block) {
		std::cout << "bruh";
		for (int i = 0; i < 4; i++)
		{
			int y = i + block.pos.y + 10;
			
			if (y >= board.size()) break;
			uint16_t line = getTetrominoLine(block.data, i) << block.pos.x;
			uint16_t boardLine = board.at(y);
			if ((line & boardLine) != 0)
			{
				return true;
			}
		}
		return false;
	}

	//ajoute la représentation 16-bits d'un tétromino à board
	void addTetromino(Tetromino& block) {
		for (int i = 0; i < 4; i++)
		{
			int y = i + block.pos.y + 10;
			if (y >= board.size()) break;
			uint16_t line = getTetrominoLine(block.data, i) << block.pos.x;
			uint16_t& boardLine = board.at(y);
			boardLine |= line;
		}
		
		for (auto& l : block.drawList())
			placedBlocks.emplace(l);
	}

	void draw(ConsoleRenderer& ren) 
	{
		/*int y = 0;
		for (auto l : board)
		{
			for (int i = 0; i < 16; i++)
			{
				uint16_t mask = (1 << i);
				bool current = l & mask;
				if (current)
				{
					V2d_i r = { (i % 16) + 16, y - 10 };
					ren.drawPixel(r);
				}
			}
			y++;
		}*/

		for (auto& l : placedBlocks)
		{
			ren.setDrawPencil(l.second);
			ren.drawPixel(l.first + V2d_i(16,0));
		}
	}
};

