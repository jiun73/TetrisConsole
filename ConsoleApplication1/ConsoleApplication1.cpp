#include "Boards.h"
#include "Tetrominos.h" 
#include "Random.h"

void drawRectPatch(const Rect& dest, ConsoleRenderer& ren)
{
	ren.setDrawGlyph((char)201);
	ren.drawPixel(dest.pos);
	ren.setDrawGlyph((char)187);
	ren.drawPixel({ dest.pos.x + dest.sz.x ,dest.pos.y });
	ren.setDrawGlyph((char)205);
	ren.drawLineHorizontal({ dest.pos.x + 1,dest.pos.y }, dest.sz.x -1 );
	ren.drawLineHorizontal({ dest.pos.x + 1,dest.pos.y + dest.sz.y  }, dest.sz.x - 1);
	ren.setDrawGlyph((char)200);
	ren.drawPixel({ dest.pos.x, dest.pos.y + dest.sz.y });
	ren.setDrawGlyph((char)188);
	ren.drawPixel({ dest.pos.x + dest.sz.x , dest.pos.y + dest.sz.y });
	ren.setDrawGlyph((char)186);
	ren.drawLineVertical({ dest.pos.x,dest.pos.y + 1 }, dest.sz.y - 1);
	ren.drawLineVertical({ dest.pos.x + dest.sz.x,dest.pos.y + 1 }, dest.sz.y - 1);
}

#include <bitset>
#include <string>

class ConsoleKeyboard 
{
private:

public:
	bool held(int code) 
	{
		return (GetKeyState(code) & 0x8000);
	}

	bool held(char c) 
	{
		return held((int)c);
	}
};

bool run = true;

class Tetris 
{
private:
	ConsoleRenderer ren;
	TetrisBoard board;
	const V2d_i blocklDF = { 6, - 4 };
	int cntr = 0;
	int speed = 10;

	int cntr2 = 0;
	int cooldown = 5;
	bool lockRotation = false;
	bool lost = false;
	bool clr = false;

	Tetromino block;
	Tetromino backup;
	ConsoleKeyboard kin;
	Random rng;

	int linesCleared = 0;
	int level = 0;
	int points = 0;

public:
	Tetris()
	{
		randomBlocks();
	}
	~Tetris() {}

	void randomBlocks() 
	{
		block.pos = blocklDF;
		block.setType(rng.range(0, 6));
		backup.setType(rng.range(0, 6));
	}

	void nextBlock() 
	{
		block = backup;
		block.pos = blocklDF;
		backup.setType(rng.range(0, 6));
	}

	void testBlockCollisionRotation()
	{
		if (board.isColliding(block))
		{
			while (true) //shitty collision detection for rotations
			{
				block.pos.x--; //try moving the block left 
				if (!board.isColliding(block)) break; //if it works, break
				block.pos.x += 2; //try moving to the right
				if (!board.isColliding(block)) break; //if it works, break
				block.pos.x++; //try 2 to the right
				if (!board.isColliding(block)) break; //if it works, break
				block.pos.x -= 4; //try 2 to the left
				if (!board.isColliding(block)) break; //if it works, break

				while (board.isColliding(block)) //is all else fails, move up until it works
					block.pos.y--;
				break;

			}
		}
	}

	void calcMovement() 
	{
		speed = ((15 - level) * 2) + 1;

		if (kin.held(VK_DOWN) && cntr2 == 0)
		{
			cntr = 99;
		}

		cntr++;
		if (cntr > speed)
		{
			cntr = 0;
			block.pos.y++;

			while (board.isColliding(block))
			{
				block.pos.y--;
				board.addTetromino(block);
				nextBlock();
				return;
			}
		}

		if (kin.held('X') && !lockRotation)
		{
			block.rotate(1);
			testBlockCollisionRotation();
			lockRotation = true;
		}

		if (kin.held('Z') && !lockRotation)
		{
			block.rotate(0);
			testBlockCollisionRotation();
			lockRotation = true;
		}

		if (!kin.held('Z') && !kin.held('X'))
			lockRotation = false;

		if (kin.held(VK_LEFT) && cntr2 == 0)
		{
			cntr2 = cooldown;
			block.pos.x--;

			while (board.isColliding(block))
				block.pos.x++;
		}

		if (kin.held(VK_RIGHT) && cntr2 == 0)
		{
			cntr2 = cooldown;
			block.pos.x++;

			if (board.isColliding(block))
				block.pos.x--;
		}

		if (!kin.held(VK_RIGHT) && !kin.held(VK_LEFT))
			cntr2 = 0;

		cntr2--;
		if (cntr2 < 0)
			cntr2 = 0;
	}

	void calcPoints() 
	{
		int cleared = board.checkFullLine();
		linesCleared += cleared;
		int reward = 0;
		switch (cleared)
		{
		case 1: reward = 100; break;
		case 2: reward = 300; break;
		case 3: reward = 500; break;
		case 4: reward = 800; break;
		default: reward = 0; break;
		}
		points += reward;

		if (linesCleared >= 10)
		{
			level++;
			linesCleared = 0;
		}


		lost = board.checkForLoss();
	}

	void gameUpdate() 
	{
		clr = false;

		ren.setDrawColor(WHITE, BG_BLACK);

		ren.setDrawGlyph(' ');
		ren.drawRect({ {20, 0 }, { 11, 21 } });

		ren.setDrawGlyph(' ');
		ren.drawRect({ {41, 2 }, { 4, 4 } });

		ren.setDrawColor(WHITE, BG_BLACK);

		calcMovement();
		calcPoints();
		board.draw(ren);

		ren.setDrawGlyph((char)219);
		block.draw(ren, { block.pos.x + 16, block.pos.y });
		backup.draw(ren, { 41, 2 });
		ren.setDrawColor(WHITE, BG_BLACK);

		drawRectPatch({ {20, 0 }, { 11, 21 } }, ren);
		drawRectPatch({ {40, 1 }, { 6, 6 } }, ren);
		ren.setDrawGlyph((char)219);
		ren.drawText("points: " + std::to_string(points), { 40, 10 });
		ren.drawText("lines: " + std::to_string(linesCleared), { 40, 11 });
		ren.drawText("level: " + std::to_string(level), { 40, 12 });

	}

	void update()
	{
		if (!lost)
			gameUpdate();
		else
		{
			if (!clr)
			{
				ren.clear();
				clr = true;
			}
			ren.drawText("You lost !", { 20, 20 });
			ren.drawText("Continue? Y/N", { 18, 21 });

			if (kin.held('Y')) 
			{
				lost = false;
				board.clear();
				randomBlocks();
			}
			else if (kin.held('N')) 
			{
				run = false;
			}
		}

		ren.present();
	}
};

int main()
{
	Tetris game;
	while (run) {
		clock_t frameStart = clock();

		game.update();

		while (clock() - frameStart < 1000 / 60) {}
	}
}