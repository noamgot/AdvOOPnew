#include "GameManager.h"


GameManager::GameManager(GetAlgoFuncType getAlgoA, GetAlgoFuncType getAlgoB, MyBoardData boardData)
{
	_playerA = unique_ptr<IBattleshipGameAlgo>(getAlgoA());
	_playerB = unique_ptr<IBattleshipGameAlgo>(getAlgoB());
	_boardData = boardData;
}


int GameManager::runGame()
{
	MyBoardData boardA(_boardData.rows(), _boardData.cols(), _boardData.depth());
	MyBoardData boardB(_boardData.rows(), _boardData.cols(), _boardData.depth());
	initIndividualBoards(boardA, boardB);


	auto attackerNum = 0, defenderNum = 1; // index 0 = A, index 1 = B
	std::unique_ptr<IBattleshipGameAlgo> pPlayers[] = { std::move(_playerA) , std::move(_playerB) };
	//TODO - init player attributes

	//The game goes on until one of the players has no more ships or both ran out of moves.
	while (_playerAttributes[0].shipsCount > 0 && _playerAttributes[1].shipsCount > 0 &&
		(_playerAttributes[0].hasMoves || _playerAttributes[1].hasMoves))
	{
		//Skip if current player is out of moves.
		if (!_playerAttributes[attackerNum].hasMoves)
		{
			GameManagerUtilities::changeAttacker(attackerNum, defenderNum);
			continue;
		}

		auto currentMove = pPlayers[attackerNum]->attack();
		if (!isLegalMove(currentMove, _boardData.rows(), _boardData.cols(), _boardData.depth()))
		{
			// Player returned a dumb move or refuses to play like a big cry baby (or maybe it has a bug)
			// we switch player and continue - even if the move is invalid (i.e we ignore invalid moves)
			GameManagerUtilities::changeAttacker(attackerNum, defenderNum);
			continue;

		}
		handleMove(_boardData, currentMove, attackerNum, defenderNum, pPlayers[PLAYER_A], pPlayers[PLAYER_B], _playerAttributes);
	}
	// TODO - figure out how this should be filled
	//_grA = PlayerGameResults(_playerAttributes[PLAYER_A].score);
	//_grB = PlayerGameResults(_playerAttributes[PLAYER_B].score;

	//printGameResults(_playerAttributes);
	return EXIT_SUCCESS;
}

void GameManager::initIndividualBoards(MyBoardData& boardA, MyBoardData& boardB) const
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
						boardB.setChar(coor, GameUtilities::WATER);
					}
					else // a ship of B
					{
						boardA.setChar(coor, GameUtilities::WATER);
						boardB.setChar(coor, c);
					}
				}
				else // a space - update both boards
				{
					boardA.setChar(coor, GameUtilities::WATER);
					boardB.setChar(coor, GameUtilities::WATER);
				}
			}
		}
	}
	
}

int GameManager::getAScore() const
{
	return _playerAttributes[PLAYER_A].score;
}

int GameManager::getBScore() const
{
	return _playerAttributes[PLAYER_B].score;
}

bool GameManager::didAWin() const
{
	return _playerAttributes[PLAYER_A].won;
}

bool GameManager::didBWin() const
{
	return _playerAttributes[PLAYER_B].won;
}
