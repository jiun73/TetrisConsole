#include "Boards.h"
#include "Tetrominos.h" 

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

	Tetromino block;

public:
	Tetris() { block.pos = blocklDF; }
	~Tetris() {}

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

	void update()
	{
		cntr++ ;
		if (cntr > speed)
		{
			cntr = 0;
			block.pos.y++;

			while (board.isColliding(block))
			{ 
				block.pos.y--;
				board.addTetromino(block);
				block.setType(rand() % 7);
				block.pos = blocklDF;
				return;
			}
		}

		ren.hideCursor();
		ren.setDrawColor(WHITE, BG_BLACK);

		ren.setDrawGlyph(' ');
		ren.drawRect({ {20, 0 }, { 11, 21 } });

		ren.setDrawColor(WHITE, BG_BLACK);

		if (GetKeyState(0x5A) & 0x8000 && !lockRotation)
		{
			block.rotate(1);
			testBlockCollisionRotation();
			lockRotation = true;
		}

		if (GetKeyState(0x58) & 0x8000 && !lockRotation)
		{
			block.rotate(0);
			testBlockCollisionRotation();
			lockRotation = true;
		}

		if (!(GetKeyState(0x58) & 0x8000) && !(GetKeyState(0x5A) & 0x8000))
			lockRotation = false;

		if (GetKeyState(VK_LEFT) & 0x8000 && cntr2 == 0)
		{
			cntr2 = cooldown;
			block.pos.x--;

			while (board.isColliding(block))
				block.pos.x++; 
		}

		if (GetKeyState(VK_RIGHT) & 0x8000 && cntr2 == 0)
		{
			cntr2 = cooldown;
			block.pos.x++;

			if (board.isColliding(block))
				block.pos.x--;
		}

		if (!(GetKeyState(VK_RIGHT) & 0x8000) && !(GetKeyState(VK_LEFT) & 0x8000))
			cntr2 = 0;

		cntr2--;
		if (cntr2 < 0)
			cntr2 = 0;

		ren.setDrawGlyph('@');
		board.draw(ren);

		ren.setDrawGlyph((char)219);
		block.draw(ren, {block.pos.x + 16, block.pos.y});
		ren.setDrawColor(WHITE, BG_BLACK);

		drawRectPatch({ {20, 0 }, { 11, 21 } }, ren);

		ren.present();
	}
};

int main()
{
	Tetris game;
	while (true) {
		clock_t frameStart = clock();

		game.update();

		while (clock() - frameStart < 1000 / 60) {}
	}
}