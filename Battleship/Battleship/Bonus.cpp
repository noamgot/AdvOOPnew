#include <iostream>
#include "Bonus.h"
#include "Player.h"

void wherexy(int& x, int& y){
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    HANDLE hStd = GetStdHandle(STD_OUTPUT_HANDLE);
    if (!GetConsoleScreenBufferInfo(hStd, &screenBufferInfo))
    {
        cout << "Error: in wherexy" << endl;
    }
    y = screenBufferInfo.dwCursorPosition.X;
    x = screenBufferInfo.dwCursorPosition.Y;
}

void gotoxy(int x, int y)
{
    COORD coord;
    coord.X = (short)y;
    coord.Y = (short)x;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void setTextColor(eColor color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

void printSign(int x, int y, eColor color, char sign, DWORD sleepTime, bool playWithGraphics)
{
    if (playWithGraphics)
    {
        int origX, origY;
        wherexy(origX, origY);
        gotoxy(x, y);
        setTextColor(color);
        std::cout << sign;
        Sleep(sleepTime);
        gotoxy(origX, origY);
        setTextColor(COLOR_DEFAULT_WHITE);
    }

}

void clearLastLine()
{
	char s[100];
	memset(s, ' ', 100); // fill with space bars
	s[99] = 0;
	cout << s << "\r";
}

void printBoard(string *board)
{
    char c;
    //clear the console and print board
    system("cls");
    for (int i = 0; i < COL_SIZE; ++i)
    {
        for (int j = 0; j < COL_SIZE; ++j)
        {
            c = board[i][j];
            if (c == WATER)
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

void printOpeningMessage()
{
	system("cls");
	cout << "*******************************\n" << \
			"*                             *\n" << \
			"*         BATTLESHIP          *\n" << \
			"*                             *\n" << \
			"*   Version 1.0, April 2017   *\n" << \
			"*   (c)all rights reserved    *\n" << \
			"*                             *\n" << \
			"*******************************";
}

