#ifndef INPUTUTILITIES_H
#define INPUTUTILITIES_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <direct.h>
#include <bitset>
#include <set>
#include <map>
#include <vector>
#include <stdlib.h>
#include <cstdio>
#include "AbstractPlayer.h"

using namespace std;

#define BAD_STRING "!@#" // for getDirPath

/* Searches the current working directory for the game configuration files:
 * .sboard -> boardPath
 * .attack-a -> atkPathA
 * .attack-b -> atkPathB */
int searchFiles(const string dirPath, string& atkPathA, string& atkPathB, string& boardPath);

/* returns an absolute path to the current working directory */
string getDirPath();

/* Initializes the battle board according to the .sboard file in boardPath.
 * results in a rows*cols board inside passed board arg */
void initBoard(const string boardPath, string* board);

/* Initializes the individual battle boards for players A and B
 * according to the board processed in initBoard */
void initIndividualBoards(string *board, char **boardA, char **boardB);

/* Checks if the ship's shape starting at board[i][j] is valid */
int checkShape(string* board, const int size, int i, int j);

/* Checks if the battle board is valid */
int checkBoardValidity(string* board);

/* Fills the attacks vector with the attack moves inside the .attack-a/b file located at atkPath */
void initAttack(const string atkPath, vector<pair<int,int>>& attacks);

#endif //INPUTUTILITIES_H
