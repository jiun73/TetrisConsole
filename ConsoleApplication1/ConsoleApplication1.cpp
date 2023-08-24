#include "Boards.h"
#include "Tetrominos.h" 

void drawRectPatch(const Rect& dest, ConsoleRenderer& ren)
{
	ren.setDrawGlyph(201);
	ren.drawPixel(dest.pos);
	ren.setDrawGlyph(187);
	ren.drawPixel({ dest.pos.x + dest.sz.x ,dest.pos.y });
	ren.setDrawGlyph(205);
	ren.drawLineHorizontal({ dest.pos.x + 1,dest.pos.y }, dest.sz.x -1 );
	ren.drawLineHorizontal({ dest.pos.x + 1,dest.pos.y + dest.sz.y  }, dest.sz.x - 1);
	ren.setDrawGlyph(200);
	ren.drawPixel({ dest.pos.x, dest.pos.y + dest.sz.y });
	ren.setDrawGlyph(188);
	ren.drawPixel({ dest.pos.x + dest.sz.x , dest.pos.y + dest.sz.y });
	ren.setDrawGlyph(186);
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
	int cooldown = 2;

	Tetromino block;

public:
	Tetris() { block.pos = blocklDF; }
	~Tetris() {}

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
				block.setType(rand() % 5);
				block.pos = blocklDF;
				return;
			}
		}

		ren.hideCursor();
		ren.setDrawColor(WHITE, BG_BLACK);

		ren.setDrawGlyph(' ');
		ren.drawRect({ {20, 0 }, { 11, 21 } });

		ren.setDrawColor(WHITE, BG_BLACK);

		if (GetKeyState(VK_LEFT) & 0x8000)
		{
			block.pos.x--;

			while (board.isColliding(block))
				block.pos.x++; 
		}

		if (GetKeyState(VK_RIGHT) & 0x8000)
		{
			block.pos.x++;

			if (board.isColliding(block))
				block.pos.x--;
		}

		ren.setDrawGlyph('@');
		board.draw(ren);
		drawRectPatch({ {20, 0 }, { 11, 21 } }, ren);

		ren.setDrawColor(GREEN, BG_BLACK);
		ren.setDrawGlyph(219);
		block.draw(ren, {block.pos.x + 16, block.pos.y});
		ren.setDrawColor(BLACK, BG_BLACK);

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