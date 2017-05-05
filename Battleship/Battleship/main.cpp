#include "GameManagerUtilities.h"
#include "NaivePlayer.h"
#include "FilePlayer.h"
#include "SmartPlayer.h"
#include "InputUtilities.h"
#include "DLLManager.h"
#include <iostream>


using namespace std;



int main(int argc, char** argv)
{
	string dirPath, boardPath, board[ROW_SIZE], dllPathA, dllPathB;
	int errorCheck = 0;
	DLLManager* dllMngr = new DLLManager;

	// allocate individual boards
	auto boardA = GameUtilities::allocateBoard(ROW_SIZE, COL_SIZE);
	auto boardB = GameUtilities::allocateBoard(ROW_SIZE, COL_SIZE);

	// processing program arguments
	GameManagerUtilities::processInputArguments(argc, argv, dirPath);

	// validate game configuration files
	if (!GameUtilities::isValidPath(dirPath))
	{
		cout << "Wrong Path: " << dirPath << endl;
		return EXIT_FAILURE;
	}
	if ((GameManagerUtilities::getBoardPath(dirPath, boardPath) < 0) ||
		(GameManagerUtilities::initGameBoards(boardPath, board, boardA, boardB) < 0))
	{
		errorCheck = 1;
	}
	if ((GameManagerUtilities::getDllPath(dirPath, dllPathA, 0) < 0) ||
		(GameManagerUtilities::getDllPath(dirPath, dllPathB, 1) < 0))
	{
		errorCheck = 1;
	}
	if (errorCheck == 1)
	{
		return EXIT_FAILURE;
	}

	PlayerAttributes playerAttributesArr[2];

	/*---------------------- New initialization for the players --------------------- */
	/* TODO: Enable it when the player algos are complete and tests on them are finished
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
	} */
	/*----------------------------------------------------------------------------- */

	IBattleshipGameAlgo *A = new FilePlayer;
	IBattleshipGameAlgo *B = new FilePlayer;

	auto retVal = GameManagerUtilities::initPlayer(A, 0, const_cast<const char **>(boardA), dirPath, playerAttributesArr);
	if (!GameManagerUtilities::initPlayer(B, 1, const_cast<const char **>(boardB), dirPath, playerAttributesArr) || !retVal)
	{
		return EXIT_FAILURE;
	}

	// delete individual boards - we do not need them anymore
	GameUtilities::deleteBoard(boardA, ROW_SIZE);
	GameUtilities::deleteBoard(boardB, ROW_SIZE);

	// Let the game begin!!!
	return GameManagerUtilities::playTheGame(A, B, playerAttributesArr, board);
}