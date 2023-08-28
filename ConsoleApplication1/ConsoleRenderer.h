#pragma once
#include <stdlib.h>
#include <Windows.h>
#include <iostream>
#include <map>
#include "Vector2D.h"
#include "Rect.h"

enum fgConsoleColors
{
	BLACK		= 0,
	DARK_BLUE	= FOREGROUND_BLUE,
	DARK_RED	= FOREGROUND_RED,
	DARK_GREEN	= FOREGROUND_GREEN,
	DARK_CYAN	= DARK_BLUE		| DARK_GREEN,
	DARK_PURPLE = DARK_BLUE		| DARK_RED,
	DARK_YELLOW = DARK_GREEN	| DARK_RED,
	GRAY		= DARK_RED		| DARK_GREEN | DARK_BLUE,
	DARK_GRAY	= FOREGROUND_INTENSITY,
	BLUE		= DARK_BLUE		| FOREGROUND_INTENSITY,
	RED			= DARK_RED		| FOREGROUND_INTENSITY,
	GREEN		= DARK_GREEN	| FOREGROUND_INTENSITY,
	CYAN		= DARK_CYAN		| FOREGROUND_INTENSITY,
	PURPLE		= DARK_PURPLE	| FOREGROUND_INTENSITY,
	YELLOW		= DARK_YELLOW	| FOREGROUND_INTENSITY,
	WHITE		= GRAY			| FOREGROUND_INTENSITY,
};

enum bgConsoleColors
{
	BG_BLACK		= 0,
	BG_DARK_BLUE	= BACKGROUND_BLUE,
	BG_DARK_RED		= BACKGROUND_RED,
	BG_DARK_GREEN	= BACKGROUND_GREEN,
	BG_DARK_CYAN	= BG_DARK_BLUE		| BG_DARK_GREEN,
	BG_DARK_PURPLE	= BG_DARK_BLUE		| BG_DARK_RED,
	BG_DARK_YELLOW	= BG_DARK_GREEN		| BG_DARK_RED,
	BG_GRAY			= BG_DARK_RED		| BG_DARK_GREEN | BG_DARK_BLUE,
	BG_DARK_GRAY	= BACKGROUND_INTENSITY,
	BG_BLUE			= BG_DARK_BLUE		| BACKGROUND_INTENSITY,
	BG_RED			= BG_DARK_RED		| BACKGROUND_INTENSITY,
	BG_GREEN		= BG_DARK_GREEN		| BACKGROUND_INTENSITY,
	BG_CYAN			= BG_DARK_CYAN		| BACKGROUND_INTENSITY,
	BG_PURPLE		= BG_DARK_PURPLE	| BACKGROUND_INTENSITY,
	BG_YELLOW		= BG_DARK_YELLOW	| BACKGROUND_INTENSITY,
	BG_WHITE		= BG_GRAY			| BACKGROUND_INTENSITY,
};

#include <array>

struct ConsolePixel
{
	bgConsoleColors bg = BG_BLACK;
	fgConsoleColors fg = BLACK;
	char glyph = (char)219;
};

class ConsoleRenderer
{
private:
	HANDLE console;

	std::map<V2d_i, ConsolePixel> screenBuffer;

	ConsolePixel pencil;

	V2d_i cursor = { 0,0 };

	V2d_i oldSize = 0;

public:
	ConsoleRenderer() 
	{ 
		console = GetStdHandle(STD_OUTPUT_HANDLE); 
	}
	~ConsoleRenderer() {}

	void setConsoleToCursor();
	void setCursor(const V2d_i& pos);
	void hideCursor();

	void setConsoleSettings(const ConsolePixel& pixel);
	void setDrawPencil(const ConsolePixel& pixel) { pencil = pixel; }
	void setConsoleColor(const fgConsoleColors& fg, const bgConsoleColors& bg) { SetConsoleTextAttribute(console, fg | bg); }
	void setDrawColor(const fgConsoleColors& fg, const bgConsoleColors& bg) { setConsoleColor(fg, bg); pencil.fg = fg; pencil.bg = bg; }
	void setDrawGlyph(const char& _glyph) { pencil.glyph = _glyph; }

	void drawLineVertical(const V2d_i& pos, const int& length);
	void drawLineHorizontal(const V2d_i& pos, const int& length);
	void drawRect(const Rect& dest);
	void drawPixel(const V2d_i& pos) { if (!isWithinScreen(pos)) return; screenBuffer[pos] = pencil; }

	void output(const std::string& out);
	void clear() { system("cls"); }

	bool isWithinScreen(const V2d_i& pos);

	V2d_i getConsoleSize();

	void present();
};