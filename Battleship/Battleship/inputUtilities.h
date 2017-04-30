#pragma once

#include <Windows.h>
#include "AbstractPlayer.h"

using namespace std;

/* Searches the current working directory for the game configuration files:
 * .sboard -> boardPath
 * .attack-a -> atkPathA
 * .attack-b -> atkPathB 
 * TODO: Delete after integration with new search completed.
 */
//int searchFiles(const string dirPath, string& atkPathA, string& atkPathB, string& boardPath);

/* Searches the current working directory for the game configuration files:
* .sboard -> boardPath
* first .dll -> dllPathA
* second .dll -> dllPathB */
int searchFilesN(const string dirPath, string& dllPathA, string& dllPathB, string& boardPath);

/* Fills the attacks vector with the attack moves inside the .attack-a/b file located at atkPath 
 * TODO: Delete after integration with new search completed.*/

int initAttackNew(const string dirPath, queue<pair<int, int>>& attacks);

/* Loads the dll in dllPath into hLib and assigns an instance of IBattleshipGameAlgo
 * to player.
 */
int loadPlayer(const string dllPath, HINSTANCE& hLib, IBattleshipGameAlgo& player);