
#include "Graphics.h"
#include <iostream>
#include "GameUtilities.h"

#define DEFAULT_LINE_CLEAR_LENGTH 100
namespace Graphics
{
	// initialize sleepTime to default value
	DWORD sleepTime = DEFAULT_GRAPHICS_DELAY;
	bool playWithGraphics = true;

	void Graphics::wherexy(int& x, int& y)
	{
		CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
		auto hStd = GetStdHandle(STD_OUTPUT_HANDLE);
		if (!GetConsoleScreenBufferInfo(hStd, &screenBufferInfo))
		{
			cout << "Error: in wherexy" << endl;
		}
		y = screenBufferInfo.dwCursorPosition.X;
		x = screenBufferInfo.dwCursorPosition.Y;
	}

	void Graphics::gotoxy(int x, int y)
	{
		COORD coord;
		coord.X = static_cast<short>(y);
		coord.Y = static_cast<short>(x);
		SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
	}

	void Graphics::printStartOfGame(const string* board, int rows, int cols)
	{
		if (playWithGraphics)
		{
			printOpeningMessage();
			Sleep(3 * DEFAULT_GRAPHICS_DELAY);
			// print the initial game board
			printBoard(board, rows, cols);
			Sleep(sleepTime);
		}
	}

	void Graphics::printBoard(const string* board, int numRows, int numCols)
	{
		char c;
		//clear the console and print board
		system("cls");
		for (auto i = 0; i < numRows; ++i)
		{
			for (auto j = 0; j < numCols; ++j)
			{
				c = board[i][j];
				if (c == GameUtilities::eShipChar::WATER)
				{
					setTextColor(COLOR_DEFAULT_WHITE);
				}
				else
				{
					setTextColor(isupper(c) ? COLOR_GREEN : COLOR_YELLOW);
				}
				cout << c;
			}
			cout << endl;
		}
		setTextColor(COLOR_DEFAULT_WHITE);
	}

	void Graphics::printOpeningMessage()
	{
		system("cls");
		cout << "*******************************\n" << \
			"*                             *\n" << \
			"*         BATTLESHIP          *\n" << \
			"*                             *\n" << \
			"*   Version 2.0, May 2017     *\n" << \
			"*                             *\n" << \
			"*        Created by:          *\n" << \
			"*        Ben Ohayon           *\n" << \
			"*        Uri Bracha           *\n" << \
			"*       Noam Gottlieb         *\n" << \
			"*                             *\n" << \
			"*   (c) all rights reserved   *\n" << \
			"*                             *\n" << \
			"*******************************";
	}

	void Graphics::printSink(char hitChar, int attackerNum, int scoreA, int scoreB)
	{

		if (playWithGraphics)
		{
			//LOG(!isupper(hitChar) == attackerNum ? "SELF-SINK\n" : "SINK\n");
			cout << (!isupper(hitChar) == attackerNum ? "SELF-SINK" : "SINK") << "\r";
			Sleep(sleepTime);
			clearLastLine();
			cout << "CURRENT SCORE: A-" << scoreA << ", B-" << scoreB << "\r";
			Sleep(sleepTime);
		}
	}

	void Graphics::printHit(bool validAttack, bool isHit, char hitChar, int attackerNum)
	{
				
		if (playWithGraphics)
		{
			if (validAttack && isHit)
			{
				//Hit xor self hit
				cout << (!isupper(hitChar) == attackerNum ? "SELF-HIT" : "HIT") << "\r";
			}
			else
			{
				cout << "ALREADY HIT\r";
			}
		}
				
	}

	void Graphics::printSign(int x, int y, eColor color, char sign)
	{
		if (playWithGraphics)
		{
			int origX, origY;
			wherexy(origX, origY);
			// need to give a (-1) offset to x,y
			gotoxy(x - 1, y - 1);
			setTextColor(color);
			cout << sign;
			Sleep(sleepTime);
			gotoxy(origX, origY);
			setTextColor(COLOR_DEFAULT_WHITE);
		}
	}

	void Graphics::clearLastLine()
	{
		char s[DEFAULT_LINE_CLEAR_LENGTH];
		memset(s, ' ', DEFAULT_LINE_CLEAR_LENGTH); // fill with space bars
		s[DEFAULT_LINE_CLEAR_LENGTH - 1] = 0;
		cout << s << "\r";
	}

	void showConsoleCursor(bool showFlag)
	{
		HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

		CONSOLE_CURSOR_INFO cursorInfo;

		GetConsoleCursorInfo(out, &cursorInfo);
		cursorInfo.bVisible = showFlag; // set the cursor visibility
		SetConsoleCursorInfo(out, &cursorInfo);
	}
}
