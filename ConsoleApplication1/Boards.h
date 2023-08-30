#pragma once
#include "TetrisGlobal.h"
#include "Tetrominos.h"
#include <vector>
#include <map>
#include <deque>
#include <bitset>

class TetrisBoard
{
private:
	//a list of 16-bit integers representing the Tetris playspace
	std::deque<uint16_t> board = {
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
		(uint16_t)0b1111111111111111,
	};

	//Les graphiques des blocs déja placé
	std::deque<std::map<int, ConsolePixel>> placedBlocks;

public:
	TetrisBoard() {  }
	~TetrisBoard() {}

	//réinitialise l'objet
	void clear()
	{
		for (int i = 0; i < board.size() - 1; i++) 
			board.at(i) = (uint16_t)0b1000000000010000;
		placedBlocks.clear();
	}

	//vérifie si il y a un tétromino placé au dessus du champ visible, signifiant que le joueur a perdu
	bool checkForLoss() 
	{
		for (int i = 0; i < 10; i++)
		{
			if (board.at(i) != 0b1000000000010000)
				return true;
		}
		return false;
	}

	//vérifie si le tétromino est en collision avec un élément dans board
	bool isColliding(Tetromino& block) {
		for (int i = 0; i < 4; i++)
		{
			int y = i + block.pos.y + 10;
			
			if (y >= board.size()) break;
			uint16_t line = getTetrominoLine(block.getData(), i) << block.pos.x;
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
			uint16_t line = getTetrominoLine(block.getData(), i) << block.pos.x;
			uint16_t& boardLine = board.at(y);
			boardLine |= line;
		}
		
		for (auto& l : block.drawList())
		{
			if (l.first.y >= 0)
			{
				placedBlocks.resize(40);
				placedBlocks[l.first.y].emplace(l.first.x, l.second);
			}
		}
	}

	//enlève un ligne et descent le reste
	void removeLine(int y) 
	{
		board.erase(board.begin() + y);
		board.push_front(0b1000000000010000);

		placedBlocks.erase(placedBlocks.begin() + y - 10);
		placedBlocks.push_front(std::map<int, ConsolePixel>());
	}

	//vérifie si il y a une (ou plus) ligne pleine, et si oui, retourne la quantité
	int checkFullLine() 
	{
		size_t i = 0;
		std::vector<size_t> indexes;
		for(auto& l : board)
		{
			if (l == 0b1111111111110000)
				indexes.push_back(i);
			i++;
		}

		for (auto& ii : indexes)
			removeLine(static_cast<int>(ii));

		return static_cast<int>(indexes.size());
	}

	//ajoute les blocks déja placé au Renderer
	void draw(ConsoleRenderer& ren, int x = 0) 
	{
		int i = 0;
		for (auto& l : placedBlocks)
		{
			for (auto& p : l)
			{
				V2d_i pos = { p.first, i };
				ren.setDrawPencil(p.second);
				ren.drawPixel(pos + V2d_i(16 + x, 0));
			}
			i++;
		}
	}
};

