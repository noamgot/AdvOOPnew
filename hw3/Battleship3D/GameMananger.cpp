#include "GameMananger.h"
#include "GameManagerUtilities.h"

int GameMananger::runGame()
{
	MyBoardData boardA(_boardData.rows(), _boardData.cols(), _boardData.depth());
	MyBoardData boardB(_boardData.rows(), _boardData.cols(), _boardData.depth());
	initIndividualBoards(boardA, boardB);


	auto attackerNum = 0, defenderNum = 1; // index 0 = A, index 1 = B
	//std::string attackerName = "A";
	std::unique_ptr<IBattleshipGameAlgo> pPlayers[] = { std::move(_A) , std::move(_B) };

	//The game goes on until one of the players has no more ships or both ran out of moves.
	while (playerAttributesArr[0].shipsCount > 0 && playerAttributesArr[1].shipsCount > 0 &&
		(playerAttributesArr[0].hasMoves || playerAttributesArr[1].hasMoves))
	{
		//Skip if current player is out of moves.
		if (!playerAttributesArr[attackerNum].hasMoves)
		{
			GameManagerUtilities::changeAttacker(attackerNum, defenderNum);
			continue;
		}

		auto currentMove = pPlayers[attackerNum]->attack();
		if (!isLegalMove(currentMove, ROW_SIZE, COL_SIZE))
		{
			if (currentMove.first == -1 && currentMove.second == -1) // an exception - means no more moves
			{
				playerAttributesArr[attackerNum].hasMoves = false; // attacker has ran out of moves
			}
			// we switch player and continue - even if the move is invalid (i.e we ignore invalid moves)
			GameManagerUtilities::changeAttacker(attackerNum, defenderNum);
			continue;

		}
		handleMove(board, currentMove, attackerNum, defenderNum, attackerName, A, B, playerAttributesArr);
	}
	//printGameResults(playerAttributesArr);
	return EXIT_SUCCESS;
}

void GameMananger::initIndividualBoards(MyBoardData& boardA, MyBoardData& boardB) const
{
	for (auto i = 0; i < _boardData.rows(); ++i)
	{
		for (auto j = 0; j < _boardData.cols(); ++j)
		{
			for (auto k = 0; k < _boardData.depth(); k++)
			{
				Coordinate coor(i, j, k);
				auto c = _boardData.charAt(coor);
				if (isalpha(c)) //part of a ship
				{
					if (isupper(c)) // a ship of A
					{
						boardA.setChar(coor, c);
						boardB.setChar(coor, WATER);
					}
					else // a ship of B
					{
						boardA.setChar(coor, WATER);
						boardB.setChar(coor, c);
					}
				}
				else // a space - update both boards
				{
					boardA.setChar(coor, WATER);
					boardB.setChar(coor, WATER);
				}
			}
		}
	}
	
}
