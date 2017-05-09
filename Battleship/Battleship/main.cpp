#include "GameManagerUtilities.h"
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
	DLLManager dllMngr;

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
	if (getBoardPath(dirPath, boardPath) < 0 || initGameBoards(boardPath, board, boardA, boardB) < 0)
	{
		errorCheck = 1;
	}
	if (getDllPath(dirPath, dllPathA, 0) < 0 || getDllPath(dirPath, dllPathB, 1) < 0)
	{
		errorCheck = 1;
	}
	if (errorCheck == 1)
	{
		return EXIT_FAILURE;
	}

	PlayerAttributes playerAttributesArr[2];
	IBattleshipGameAlgo *A;
	IBattleshipGameAlgo *B;

	if (!initPlayer(A, 0, const_cast<const char **>(boardA), dirPath, dllPathA, playerAttributesArr, dllMngr) ||
		!initPlayer(B, 1, const_cast<const char **>(boardB), dirPath, dllPathB, playerAttributesArr, dllMngr))
	{
		return EXIT_FAILURE;
	} 

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
	

	// Let the game begin!!!
	auto ret = playTheGame(A, B, playerAttributesArr, board);

	return ret;
}