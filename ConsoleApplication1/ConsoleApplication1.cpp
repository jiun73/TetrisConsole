#include "Boards.h"
#include "Tetrominos.h" 
#include "Random.h"
#include "NetworkingX.h"
#include "FileInterpret.h"

bool peer = false;
bool restart = false;

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
#include <sstream>

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
	std::list<int> nextPieces;
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

	std::vector<int> garbageBuffer;

public:
	Tetris() { randomBlocks(); }
	~Tetris() {}

	bool isPeer = false;

	void pushGarbageBuffer() 
	{
		sendLines(garbageBuffer);
		board.addGarbagelineList(garbageBuffer);
	}

	void addGarbageToBuffer(std::vector<int>& list)
	{
		for (auto& l : list)
			garbageBuffer.push_back(l);
	}

	void fillNextPieces() 
	{
		std::vector<int> all = {0,1,2,3,4,5,6};
		for (int i = 0; i < 7; i++)
		{
			int n = rng.range(0, all.size() - 1);
			nextPieces.push_back(all.at(n));
			all.erase(all.begin() + n);
		}
	}

	void randomBlocks() 
	{
		fillNextPieces();
		block.pos = blocklDF;
		block.setType(nextPieces.back());
		nextPieces.pop_back();
		backup.setType(nextPieces.back());
		nextPieces.pop_back();
	}

	void nextBlock() 
	{
		pushGarbageBuffer();
		block = backup;
		block.pos = blocklDF;
		if(nextPieces.empty())
			fillNextPieces();
		backup.setType(nextPieces.back());
		nextPieces.pop_back();
	}

	virtual void onBlockPlacement(Tetromino& block) { firstPlaced = true; }

	virtual void sendLines(std::vector<int>& list) {}

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

	std::vector<int> getGarbage(int n) 
	{
		std::vector<int> ret;
		for (int i = 0; i < n; i++)
			ret.push_back(rng.range(0, 9));
		return ret;
	}

	void calcPoints(TetrisBoard& b, int& _cleared, int& _points, int& tspin, int& _combo, int& _b2b, int& _linesToSend, bool& _loss, int _level, bool& _first)
	{
		_cleared = b.checkFullLine();
		bool difficult = false;

		if (tspin)
			difficult = true;

		if (_cleared)
		{
			_combo++;

			if (_cleared == 4)
				difficult = true;
		}
		else
			_combo = -1;

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
			
			switch (_cleared)
			{
			case 1:_linesToSend = 0; break;
			case 2:_linesToSend = 1; break;
			case 3:_linesToSend = 2; break;
			case 4:_linesToSend = 4; break;
			}
		}
		else if (_cleared && tspin) 
		{
			switch (_cleared)
			{
			case 1:_linesToSend = 2; break;
			case 2:_linesToSend = 4; break;
			case 3:_linesToSend = 6; break;
			}
		}

		if (_linesToSend > 0 && _b2b > 0)
			_linesToSend = _linesToSend + _b2b;

		if(_cleared)
			_points += (reward + (50 * _combo * _level) ) * ((_b2b > 0) ? 1.5 : 1.0);

		if (tspin)
		{
			difficult = true;
			tspin = 0;
		}

		if (_first && board.isEmpty())
		{
			_linesToSend = 10 + _cleared;
			_first = false;
		}

		_loss = b.checkForLoss();
	}

	virtual void gameUpdate() 
	{
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

		ren.setDrawGlyph(' ');
		ren.drawRect({ {40, 10 }, { 15, 5} });

		ren.setDrawColor(WHITE, BG_BLACK);

		calcMovement();
		int cleared = 0;
		int linesToSend = 0;
		bool wasTspin = tspinVal;
		calcPoints(board, cleared, points, tspinVal, combo, backtoback, linesToSend, lost, level, firstPlaced);

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

	virtual void onLose() 
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
			resetAll();
		}
		else if (kin.held('N'))
		{
			run = false;
		}
	}

	virtual void update()
	{
		if (!lost)
			gameUpdate();
		else
			onLose();

		ren.present();
	}

	void resetAll() 
	{
		firstPlaced = false;
		lastMoveWasRotation = false;
		lastClearWasTSpin = 0;

		linesCleared = 0;
		level = 0;
		points = 0;

		combo = -1;
		backtoback = -1;
		tspinVal = 0;

		lost = false;
		if (save != nullptr)
			delete save;
		save = nullptr;
		points = 0;
		tspinVal = 0;
		board.clear();
		nextPieces.clear();
		randomBlocks();
		ren.clear();
	}
};

class TetrisMultiplayer : public Tetris
{
private:
	NetworkingX net;
	bool _init = false;
	TetrisBoard boardOpp;
	bool win = false;

	int wins = 0;
	int losses = 0;

public:
	void sendLines(std::vector<int>& list) override
	{
		net.send(1);
		net.send(list.size());
		for (auto& l : list)
			net.send(l);
		net.signal(3);
	}

	void onBlockPlacement(Tetromino& block) override
	{
		net.send(0);
		net.send(block.getType());
		net.send(block.pos);
		net.send(block.getRotation());
		net.send(tspinVal);
		net.send(firstPlaced);
		net.signal(3);
	}

	void onLose() override 
	{
		if (!clr)
		{
			ren.clear();
			losses++;

			clr = true;
		}

		std::stringstream ss;
		ss << wins << " wins and " << losses << " losses";
		ren.drawText(ss.str(), { 20, 23 });

		ren.drawText("You lose >:)", { 20, 20 });
		ren.drawText("Rematch? Y/N", { 17, 21 });

		if (kin.held('Y'))
		{
			net.send(3);
			net.signal(3);

			while (net.hasSignal(3))
			{
				if (net.read<int>() == 3)
				{
					
					resetAll();
				}
			}
		}
		else if (kin.held('N'))
		{
			run = false;
		}
	}

	void onWin()
	{
		if (!clr)
		{
			ren.clear();

			wins++;

			clr = true;
		}

		std::stringstream ss;
		ss << wins << " wins and " << losses << " losses";
		ren.drawText(ss.str(), { 20, 23 });

		ren.drawText("You WIN >:)", { 20, 20 });
		ren.drawText("Rematch? Y/N", { 17, 21 });
		if (kin.held('Y'))
		{
			net.send(3);
			net.signal(3);

			while (net.hasSignal(3))
			{
				if (net.read<int>() == 3)
				{
					win = false;
					resetAll();
				}
			}
		}
		else if (kin.held('N'))
		{
			run = false;
		}
	}

	void update() override
	{

		if (!win) {
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
						bool fplaced = net.read<bool>();

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
						calcPoints(boardOpp, cleared, _points, tspinValue, _combo, _b2b, linesToSend, loss, 0, fplaced);

						std::vector<int> list = getGarbage(linesToSend);
						//sendLines(list);
						//board.addGarbagelineList(list);
						addGarbageToBuffer(list);

						if (loss) //means that we win
						{
							net.send(2);
							net.signal(3);
							win = true;
						}

					}
					else if (messageType == 1) {
						size_t size = net.read<size_t>();
						std::vector<int> list;
						for (int i = 0; i < size; i++)
							list.push_back(net.read<int>());

						boardOpp.addGarbagelineList(list);
					}
					else if (messageType == 2) //we lose
					{
						lost = true;
					}
				}

				ren.setDrawColor(BLACK, BG_BLACK);
				ren.setDrawGlyph(' ');
				ren.drawRect({ {60, 0 }, { 11, 21 } });
				boardOpp.draw(ren, 40);
				ren.setDrawColor(WHITE, BG_BLACK);
				drawRectPatch({ {60, 0 }, { 11, 21 } }, ren);
				Tetris::update();

				if (net.disconnected())
				{
					ren.clear();
					std::cout << "Peer disconnected\n";
					system("pause");
					run = false;
				}

			}
		}
		else
		{
			onWin();
		}
	}

	void init() 
	{
		std::cout << "Host? Y/N" << std::endl;
		bool skip = false;
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
			std::cout << "ip? (enter CHOOSE for a list of previouly entered ips)\n";
			std::string s;
			std::cin >> s;
			if (s.compare("CHOOSE") == 0)
			{
				std::vector<std::string> collectedIps;
				File f("savedIps.sav");
				if (f.open_read())
				{
					while (f.get_cursor() < f.size())
						collectedIps.push_back(f.readwrite_string("", "ip"));
				}

				int i = 0;
				for (auto& ip : collectedIps)
				{
					std::cout << i << ": " << ip << "\n";
					i++;
				}

				if (!collectedIps.empty())
				{
					int chosenIpIndex = 0;
					std::cin >> chosenIpIndex;

					s = collectedIps.at(chosenIpIndex);
				}
				else
				{
					std::cout << "There was no saved ips\n";
					skip = true;
				}
			}


			if (!skip) {
				bool joinSuccessfull = net.join();
				if (joinSuccessfull)
				{
					File f("savedIps.sav");
					f.open_write();
					f.move_cursor_to(f.size());
					f.readwrite_string(s, "ip");

					_init = true;
				}
			}
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