#include "GameManagerUtilities.h"
#include "NaivePlayer.h"
#include "FilePlayer.h"
#include "SmartPlayer.h"
#include "DLLManager.h"
#include <iostream>
#include "Graphics.h"


using namespace std;
using namespace GameManagerUtilities;
using namespace GameUtilities;



int main(int argc, char** argv)
{
	string dirPath, boardPath, board[ROW_SIZE], dllPathA, dllPathB;
	auto errorCheck = 0;
	auto dllMngr = new DLLManager; //TODO @Ben - why pointer and not an instance? better avoid dynamic allocations...

	//hide the console cursur
	Graphics::showConsoleCursor(false);

	// allocate individual boards
	auto boardA = allocateBoard(ROW_SIZE, COL_SIZE);
	auto boardB = allocateBoard(ROW_SIZE, COL_SIZE);

	// processing program arguments
	if (processInputArguments(argc, argv, dirPath) < 0)
	{
		return EXIT_FAILURE;
	}

	// validate game configuration files
	if (!isValidPath(dirPath))
	{
		cout << "Wrong Path: " << dirPath << endl;
		return EXIT_FAILURE;
	}
	if (getBoardPath(dirPath, boardPath) < 0 ||
		initGameBoards(boardPath, board, boardA, boardB) < 0)
	{
		errorCheck = 1;
	}
	/*
	if (GameManagerUtilities::getDllPath(dirPath, dllPathA, 0) < 0 ||
		GameManagerUtilities::getDllPath(dirPath, dllPathB, 1) < 0)
	{
		errorCheck = 1;
	}*/
	if (errorCheck == 1)
	{
		return EXIT_FAILURE;
	}

	PlayerAttributes playerAttributesArr[2];

	/*---------------------- New initialization for the players --------------------- */
	// Initialize player one
	IBattleshipGameAlgo *A = dllMngr->loadAlgo(dllPathA);
	if (A == nullptr)
	{
		return EXIT_FAILURE;
	}
	if (!GameManagerUtilities::initPlayer(A, 0, const_cast<const char **>(boardA), dirPath, playerAttributesArr))
	{
		cout << "Algorithm initialization failed for dll: " << dllPathA << endl;
		return EXIT_FAILURE;
	}
	// Initialize player two
	IBattleshipGameAlgo *B = dllMngr->loadAlgo(dllPathB);
	if (B == nullptr)
	{
		return EXIT_FAILURE;
	}
	if (!GameManagerUtilities::initPlayer(B, 1, const_cast<const char **>(boardB), dirPath, playerAttributesArr))
	{
		cout << "Algorithm initialization failed for dll: " << dllPathB << endl;
		return EXIT_FAILURE;
	} 
	/*----------------------------------------------------------------------------- */

	/* ------------------- Old Player init ------------------------------ 
	IBattleshipGameAlgo *A = new FilePlayer;
	IBattleshipGameAlgo *B = new SmartPlayer;

	if (!initPlayer(A, 0, const_cast<const char **>(boardA), dirPath, playerAttributesArr) ||
		!initPlayer(B, 1, const_cast<const char **>(boardB), dirPath, playerAttributesArr))
	{
		return EXIT_FAILURE;
	}
	-----------------------------------------------------------------------*/

	// delete individual boards - we do not need them anymore
	deleteBoard(boardA, ROW_SIZE);
	deleteBoard(boardB, ROW_SIZE);
	delete dllMngr;

	// Let the game begin!!!
	return playTheGame(A, B, playerAttributesArr, board);
}