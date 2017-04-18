#ifndef HW1_BONUS_H
#define HW1_BONUS_H

#include <windows.h>
using namespace std;


#define DEFAULT_SLEEP_TIME 1000 //milliseconds
#define BOMB_SIGN '@'
#define HIT_SIGN '*'

typedef enum _eColor
{
    COLOR_DEFAULT_WHITE = 7,
    COLOR_GREEN = 10,
    COLOR_RED = 12,
    COLOR_YELLOW = 14,
}eColor;

/*changes the console output color according to the given color*/
void setTextColor(eColor color);

/*gets the current console cursor coordinates*/
void wherexy(int& x, int& y);

/*sets the console cursor to the given coordinates*/
void gotoxy(int x, int y);

/*prints the initial game board*/
void printBoard(string *board);

void printOpeningMessage();

/*if playWithGraphics == true: prints the given sign with the given color
 * in the given coordinates(x,y) and waits for sleepTime milliseconds*/
void printSign(int x, int y, eColor color, char sign, DWORD sleepTime, bool playWithGraphics);

void clearLastLine();
#endif //HW1_BONUS_H
