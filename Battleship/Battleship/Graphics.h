#pragma once

#include <windows.h>
#include <string>
using namespace std;

#define HIT_SIGN_TO_DELETE '*'

enum eColor
{
	COLOR_DEFAULT_WHITE = 7,
	COLOR_GREEN = 10,
	COLOR_RED = 12,
	COLOR_YELLOW = 14,
};
class Graphics
{
public:
	// consts
	static const char BOMB_SIGN;
	static const char HIT_SIGN;
	static const int DEFAULT_GRAPHICS_DELAY; //millliseconds
	
	//data members
	static DWORD sleepTime;
	static bool playWithGraphics;

	// class static methods

	/*changes the console output color according to the given color*/
	static void setTextColor(eColor color);

	/*gets the current console cursor coordinates*/
	static void wherexy(int& x, int& y);

	/*sets the console cursor to the given coordinates*/
	static void gotoxy(int x, int y);

	/*prints the initial game board*/
	static void printBoard(const string* board, int numRows, int numCols);

	static void printOpeningMessage();

	/*if playWithGraphics == true: prints the given sign with the given color
	 * in the given coordinates(x,y) and waits for sleepTime milliseconds*/
	static void printSign(int x, int y, eColor color, char sign);

	static void clearLastLine();
};