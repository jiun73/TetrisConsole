#include "ConsoleRenderer.h"

void ConsoleRenderer::setConsoleToCursor()
{
	COORD pos = { (short)cursor.x, (short)cursor.y };
	SetConsoleCursorPosition(console, pos);
}

void ConsoleRenderer::setCursor(const V2d_i& pos)
{
	cursor = pos;
	setConsoleToCursor();
}

void ConsoleRenderer::setConsoleSettings(const ConsolePixel& pixel)
{
	pencil = pixel;
	setConsoleColor(pixel.fg, pixel.bg);
}

void ConsoleRenderer::drawLineVertical(const V2d_i& pos, const int& length)
{
	for (int y = 0; y < length; y++)
		drawPixel({ pos.x, pos.y + y });
}

void ConsoleRenderer::drawLineHorizontal(const V2d_i& pos, const int& length)
{
	for (int x = 0; x < length; x++)
		drawPixel({ x + pos.x, pos.y });
}

void ConsoleRenderer::drawRect(const Rect& dest)
{
	for (int y = 0; y < dest.sz.y; y++)
		for (int x = 0; x < dest.sz.x; x++)
			drawPixel({ x + dest.pos.x,y + dest.pos.y });
}

void ConsoleRenderer::hideCursor()
{
	CONSOLE_CURSOR_INFO info;
	info.dwSize = 100;
	info.bVisible = FALSE;
	SetConsoleCursorInfo(console, &info);
}

void ConsoleRenderer::output(const std::string& out)
{
	setCursor({ 0, 30 });
	setDrawColor(WHITE, BG_BLACK);
	for (auto& c : out)
		std::cout << c;
	//currentOutputLine++;
}

bool ConsoleRenderer::isWithinScreen(const V2d_i& pos)
{
	if (pos.x < 0) return false;
	if (pos.y < 0) return false;

	V2d_i consoleSize = getConsoleSize();

	if (pos.x >= consoleSize.x) return false;
	if (pos.y >= consoleSize.y) return false;

	return true;
}

V2d_i ConsoleRenderer::getConsoleSize()
{
	V2d_i size;
	CONSOLE_SCREEN_BUFFER_INFO buff;
	GetConsoleScreenBufferInfo(console, &buff);
	size.x = buff.srWindow.Right - buff.srWindow.Left + 1;
	size.y = buff.srWindow.Bottom - buff.srWindow.Top + 1;

	return size;
}

void ConsoleRenderer::present()
{
	V2d_i newSize = getConsoleSize() - 1;
	if (oldSize != newSize)
	{
		clear();
		oldSize = newSize;
	}

	setCursor(0);
	for (auto& o : screenBuffer)
	{
		if (isWithinScreen(o.first))
		{
			setCursor(o.first);
			setConsoleSettings(o.second);
			std::cout << o.second.glyph;
		}
	}
	screenBuffer.clear();
}
