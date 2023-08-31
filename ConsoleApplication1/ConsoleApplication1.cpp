#include "Boards.h"
#include "Tetrominos.h" 
#include "Random.h"
#include "NetworkingX.h"

bool peer = false;

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
protected:
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

	bool saveLock = false;

	bool flash = false;
	bool firstPlaced = false;
	bool lastMoveWasRotation = false;
	int  lastClearWasTSpin = 0;

	Tetromino block;
	Tetromino backup;
	Tetromino* save = nullptr;
	ConsoleKeyboard kin;
	Random rng;

	int linesCleared = 0;
	int level = 0;
	int points = 0;

	int combo = -1;
	int backtoback = -1;
	int tspinVal = 0;

public:
	Tetris() { randomBlocks(); }
	~Tetris() {}

	bool isPeer = false;

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

	virtual void onBlockPlacement(Tetromino& block) { firstPlaced = true; }

	virtual void sendLines(int n) {}

	void calcMovement() 
	{
		speed = ((15 - level) * 2) + 1;

		if (kin.held(VK_DOWN) && cntr2 == 0)
			cntr = 99;

		cntr++;
		if (cntr > speed)
		{
			cntr = 0;
			block.pos.y++;

			while (board.isColliding(block))
			{
				block.pos.y--;
				tspinVal = board.addTetromino(block, lastMoveWasRotation);
				std::cout << tspinVal << std::endl;
				onBlockPlacement(block);
				nextBlock();
				saveLock = false;
				return;
			}
			lastMoveWasRotation = false;
		}

		if (kin.held('X') && !lockRotation)
		{
			block.rotate(1, board);
			lastMoveWasRotation = true;
			lockRotation = true;
		}

		if (kin.held('Z') && !lockRotation)
		{
			block.rotate(0, board);
			lastMoveWasRotation = true;
			lockRotation = true;
		}

		if (!kin.held('Z') && !kin.held('X'))
			lockRotation = false;

		if (kin.held(VK_LEFT) && cntr2 == 0)
		{
			lastMoveWasRotation = false;
			block.move(1, board);
			cntr2 = cooldown;
		}

		if (kin.held(VK_RIGHT) && cntr2 == 0)
		{
			lastMoveWasRotation = false;
			block.move(0, board);
			cntr2 = cooldown;
		}

		if (!kin.held(VK_RIGHT) && !kin.held(VK_LEFT))
			cntr2 = 0;

		if (kin.held('C') && !saveLock)
		{
			lastMoveWasRotation = false;
			Tetromino temp;
			bool wasNull = true;
			if (save != nullptr)
			{
				temp = *save;
				delete save;
				wasNull = false;
			}

			save = new Tetromino(block);

			if (!wasNull)
			{
				block = temp;
				block.pos = blocklDF;
			}
			else
				nextBlock();

			saveLock = true;
		}

		cntr2--;
		if (cntr2 < 0)
			cntr2 = 0;
	}

	void calcPoints(TetrisBoard& b, int& _cleared, int& _points, int& tspin, int& _combo, int& _b2b, int& _linesToSend, bool& _loss, int _level)
	{
		_cleared = b.checkFullLine();
		bool difficult = false;

		if (tspin)
			difficult = true;

		if (_cleared)
		{
			//flash = true;
			_combo++;

			if (_cleared == 4)
				difficult = true;
		}
		else
			_combo = -1;

		//if (tspin && _cleared)
		//	wasTspin = _cleared;
		//else if (!tspin && _cleared)
		//	wasTspin = false;

		int reward = 0;
		if (!tspin)
		{
			switch (_cleared)
			{
			case 1: reward = 100; break;
			case 2: reward = 300; break;
			case 3: reward = 500; break;
			case 4: reward = 800; break;
			default: reward = 0; break;
			}
		}
		else
		{
			switch (_cleared)
			{
			case 1: reward = 800; break;
			case 2: reward = 1200; break;
			case 3: reward = 1600; break;
			default: reward = 400; break;
			}
		}

		if (difficult) 
			_b2b++;
		else
			_b2b = -1;

		if(_cleared && !tspin)
		{
			_linesToSend = _cleared;
		}

		if(_cleared)
			_points += (reward + (50 * _combo * _level) ) * ((_b2b > 0) ? 1.5 : 1.0);

		if (tspin)
		{
			difficult = true;
			tspin = 0;
		}

		_loss = b.checkForLoss();
	}

	virtual void gameUpdate() 
	{
		std::cout << block.getType() << std::endl;

		clr = false;
		if(save != nullptr)
			save->pos = 0;

		ren.setDrawColor(WHITE, BG_BLACK);

		if(flash)
		{
			ren.setDrawColor(WHITE, BG_WHITE);
			flash = false;
		}
		ren.setDrawGlyph(' ');
		ren.drawRect({ {20, 0 }, { 11, 21 } });

		ren.setDrawGlyph(' ');
		ren.drawRect({ {41, 2 }, { 4, 4 } });

		ren.setDrawGlyph(' ');
		ren.drawRect({ {0, 0 }, { 4, 4 } });

		ren.setDrawColor(WHITE, BG_BLACK);

		calcMovement();
		int cleared = 0;
		int linesToSend = 0;
		bool wasTspin = tspinVal;
		calcPoints(board, cleared, points, tspinVal, combo, backtoback, linesToSend, lost, level);

		if (wasTspin && cleared)
			lastClearWasTSpin = cleared;
		else if (!wasTspin && cleared)
			lastClearWasTSpin = false;

		if (linesCleared >= 10)
		{
			level++;
			linesCleared = 0;
		}

		if (cleared)
			flash = true;


		linesCleared += cleared;
		board.draw(ren);

		if (firstPlaced && board.isEmpty())
			firstPlaced = false;

		ren.setDrawGlyph((char)219);
		block.draw(ren, { block.pos.x + 16, block.pos.y });
		backup.draw(ren, { 41, 2 });
		if(save != nullptr)
			save->draw(ren, 0);
		ren.setDrawColor(WHITE, BG_BLACK);

		drawRectPatch({ {20, 0 }, { 11, 21 } }, ren);
		drawRectPatch({ {40, 1 }, { 6, 6 } }, ren);
		ren.setDrawGlyph((char)219);
		ren.drawText("points: " + std::to_string(points), { 40, 10 });
		ren.drawText("lines: " + std::to_string(linesCleared), { 40, 11 });
		ren.drawText("level: " + std::to_string(level), { 40, 12 });
		if(combo > 1)
			ren.drawText("combo: " + std::to_string(combo), { 40, 13 });
		if (backtoback > 1)
			ren.drawText("backtoback: " + std::to_string(backtoback), { 40, 14 });
		if(lastClearWasTSpin > 0)
			ren.drawText(std::to_string(lastClearWasTSpin) + " T-Spin", {40, 13 + (combo > 1) + (backtoback > 1)});

	}

	virtual void update()
	{
		if (!lost)
			gameUpdate();
		else
		{
			if (!clr)
			{
				if(save != nullptr)
					delete save;
				save = nullptr;
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
				ren.clear();
			}
			else if (kin.held('N')) 
			{
				run = false;
			}
		}

		ren.present();
	}
};

class TetrisMultiplayer : public Tetris
{
private:
	NetworkingX net;
	bool _init = false;
	TetrisBoard boardOpp;

public:
	void sendLines(int n) override
	{
		net.send(1);
		net.send(n);
		net.signal(3);
	}

	void onBlockPlacement(Tetromino& block) override
	{
		net.send(0);
		net.send(block.getType());
		net.send(block.pos);
		net.send(block.getRotation());
		net.send(tspinVal);
		net.signal(3);
	}

	void update() override
	{
		if (!_init)
			init();
		else 
		{
			while (net.hasSignal(3)) 
			{
				int messageType = net.read<int>();

				if (messageType == 0) {
					int type = net.read<int>();
					V2d_i pos = net.read<V2d_i>();
					int rotation = net.read<int>();
					int tspinValue = net.read<int>();

					Tetromino nBlock;
					nBlock.setType(type);
					nBlock.pos = pos;
					nBlock.setRotation(rotation);
					boardOpp.addTetromino(nBlock);

					int cleared = 0;
					int linesToSend = 0;
					bool wasTspin = tspinVal;
					int _points = 0;
					int _combo = 0;
					int _b2b = 0;
					bool loss = 0;
					calcPoints(boardOpp, cleared, _points, tspinValue, _combo, _b2b, linesToSend, loss, 0);
					//sendLines(linesToSend);
					//boardOpp.addGarbageLine(linesToSend);

				}
				else if (messageType == 1) {
					int garbage = net.read<int>();
					//board.addGarbageLine(garbage);
				}

			}

			ren.setDrawColor(BLACK, BG_BLACK);
			ren.setDrawGlyph(' ');
			ren.drawRect({ {60, 0 }, { 11, 21 } });
			boardOpp.draw(ren, 40);
			ren.setDrawColor(WHITE, BG_BLACK);
			drawRectPatch({ {60, 0 }, { 11, 21 } }, ren);
			Tetris::update();

		}
	}

	void init() 
	{
		std::cout << "Host? Y/N" << std::endl;
		char c;
		std::cin >> c;

		if (c == 'Y') 
		{
			net.host();
			net.wait_for_peer();
			_init = true;
		}
		else if (c == 'N')
		{
			std::cout << "ip? (leave empty for localhost)";
			std::string s;
			std::cin >> s;
			if (s.empty())
				_init = net.join();
			else
				_init = net.join(s);
		}
		else
		{
			std::cout << "Invalid response!" << std::endl;
			run = false;
		}
	}
};

int main()
{
	std::cout << "1. Singleplayer\n2. Multiplayer\n";
	int n;
	std::cin >> n;

	Tetris* game = nullptr;

	if (n == 1)
		game = new Tetris();
	else if(n == 2)
		game = new TetrisMultiplayer();
	else
		run = false;

	while (run) {
		clock_t frameStart = clock();

		game->update();

		while (clock() - frameStart < 1000 / 30) {} //locks updates at 60 fps
	}
}