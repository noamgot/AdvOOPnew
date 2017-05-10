#include "GameManagerUtilities.h"
#include "DLLManager.h"
#include "Graphics.h"



using namespace std;
using namespace GameManagerUtilities;
using namespace GameUtilities;

int main(int argc, char** argv)
{
	// Seed the PRG for all of our random needs.
	srand(static_cast<unsigned int>(time(nullptr))); //todo @Uri - why is this in main?!
	string dirPath, boardPath, board[ROW_SIZE], dllPathA, dllPathB;
	DLLManager dllMngr;

	//hide the console cursor
	Graphics::showConsoleCursor(false);

	// allocate individual boards
	auto boardA = allocateBoard(ROW_SIZE, COL_SIZE), boardB = allocateBoard(ROW_SIZE, COL_SIZE);

	// initialize game, boards and paths
	if (initGameBoardsAndPaths(argc ,argv, board, boardA, boardB, dirPath, dllPathA, dllPathB) < 0)
	{
		return EXIT_FAILURE;
	}
	
	// initialize players and player attributes
	PlayerAttributes playerAttributesArr[2];
	IBattleshipGameAlgo *A, *B;
	if (!initPlayer(A, 0, const_cast<const char **>(boardA), dirPath, dllPathA, playerAttributesArr, dllMngr) ||
		!initPlayer(B, 1, const_cast<const char **>(boardB), dirPath, dllPathB, playerAttributesArr, dllMngr))
	{
		return EXIT_FAILURE;
	} 

	// delete individual boards - we do not need them anymore
	deleteBoard(boardA, ROW_SIZE);
	deleteBoard(boardB, ROW_SIZE);

	// Let the game begin!!!
	return playTheGame(A, B, playerAttributesArr, board);
}