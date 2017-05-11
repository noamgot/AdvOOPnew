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
namespace Graphics
{
	// consts
	const char BOMB_SIGN = '@';
	const char HIT_SIGN = '*';
	const int DEFAULT_GRAPHICS_DELAY = 1000; //millliseconds

   // initialize sleepTime to default value
	extern DWORD sleepTime;
	extern bool playWithGraphics;

	// namespace  methods

	/*changes the console output color according to the given color*/
	void setTextColor(eColor color);

	/*gets the current console cursor coordinates*/
	void wherexy(int& x, int& y);

	/*sets the console cursor to the given coordinates*/
	void gotoxy(int x, int y);

	/* helper functions for printing */
	void printStartOfGame(const string *board, int rows, int cols);
	void printBoard(const string* board, int numRows, int numCols);
	void printOpeningMessage();
	void printSink(char hitChar, int attackerNum, int scoreA, int scoreB);
	void printHit(bool validAttack, bool isHit, char hitChar, int attackerNum);

	/*if playWithGraphics == true: prints the given sign with the given color
	 * in the given coordinates(x,y) and waits for sleepTime milliseconds*/
	void printSign(int x, int y, eColor color, char sign);

	void clearLastLine();

	/*if showFlag == true - shows the console cursur; if showFlag == false - hides the cursur*/
	void showConsoleCursor(bool showFlag);

}