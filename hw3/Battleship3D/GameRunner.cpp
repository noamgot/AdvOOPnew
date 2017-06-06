#include "GameRunner.h"
#include <iso646.h>

using namespace std;

GameRunner::GameRunner(const Game& game, const GetAlgoFuncType& getAlgoA, const GetAlgoFuncType& getAlgoB, const MyBoardData& boardData, std::shared_ptr<Logger> pLogger)
	:_game(game), _playerA(getAlgoA()), _playerB(getAlgoB()), _pLogger(pLogger)
{
	
	initPlayersAttributes(boardData, PLAYER_A);
	initPlayersAttributes(boardData, PLAYER_B);
	_boardData = boardData;
	_pLogger->writeToLog("Initialized game: boardID = " + to_string(game._boardID) + ", A=" + to_string(game._idA) + ", B: " + to_string(game._idB));
}


vector<PlayerGameResults> GameRunner::runGame()
{
	// Copy board to personal boards
	vector<MyBoardData> mbd(2);
	mbd[0] = MyBoardData(_boardData.rows(), _boardData.cols(), _boardData.depth());
	mbd[1] = MyBoardData(_boardData.rows(), _boardData.cols(), _boardData.depth());
	int counter = 0;
	initIndividualBoards(mbd[PLAYER_A], mbd[PLAYER_B]);



	auto attackerNum = 0, defenderNum = 1; // index 0 = A, index 1 = B
	std::unique_ptr<IBattleshipGameAlgo> pPlayers[] = { std::move(_playerA) , std::move(_playerB) };
	for (auto player_id = 0; player_id < PLAYER_COUNT; player_id++)
	{
		pPlayers[player_id]->setPlayer(player_id);
		pPlayers[player_id]->setBoard(mbd[player_id]);
	}
	//The game goes on until one of the players has no more ships or both ran out of moves.
	while (_playerAttributes[0].shipsCount > 0 && _playerAttributes[1].shipsCount > 0 &&
		(_playerAttributes[0].hasMoves || _playerAttributes[1].hasMoves))
	{
		// Skip if current player is out of moves.
		if (!_playerAttributes[attackerNum].hasMoves)
		{
			changeAttacker(attackerNum, defenderNum);
			continue;
		}


		auto currentMove = pPlayers[attackerNum]->attack();
		if (!CommonUtilities::isLegalMove(currentMove, _boardData.rows(), _boardData.cols(), _boardData.depth()))
		{
			// Player returned a dumb move or refuses to play like a big cry baby (or maybe it has a bug)
			// we switch player and continue - even if the move is invalid (i.e we ignore invalid moves)
			_playerAttributes[attackerNum].hasMoves = !(currentMove.row == -1 && currentMove.col == -1 && currentMove.depth == -1);
			GameRunner::changeAttacker(attackerNum, defenderNum);
			continue;

		}
		handleMove(_boardData, currentMove, attackerNum, defenderNum, pPlayers[PLAYER_A], pPlayers[PLAYER_B], _playerAttributes);
	}
	_pLogger->writeToLog("Finished game: boardID = " + to_string(_game._boardID) + ", A=" + to_string(_game._idA) + ", B: " + to_string(_game._idB));
	vector<PlayerGameResults> pgr(2);
	pgr[PLAYER_A] = PlayerGameResults(_game._idA, _playerAttributes[PLAYER_A].won, 1 - _playerAttributes[PLAYER_A].won, _playerAttributes[PLAYER_A].score, _playerAttributes[PLAYER_B].score, 100 * _playerAttributes[PLAYER_A].won);
	pgr[PLAYER_B] = PlayerGameResults(_game._idA, _playerAttributes[PLAYER_B].won, 1 - _playerAttributes[PLAYER_B].won, _playerAttributes[PLAYER_B].score, _playerAttributes[PLAYER_B].score, 100 * _playerAttributes[PLAYER_B].won);
	return pgr;
}

void GameRunner::initIndividualBoards(MyBoardData& boardA, MyBoardData& boardB) const
{
	//todo - start loops from 1 ?
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
						boardB.setChar(coor, CommonUtilities::WATER);
					}
					else // a ship of B
					{
						boardA.setChar(coor, CommonUtilities::WATER);
						boardB.setChar(coor, c);
					}
				}
				else // a space - update both boards
				{
					boardA.setChar(coor, CommonUtilities::WATER);
					boardB.setChar(coor, CommonUtilities::WATER);
				}
			}
		}
	}
	
}

//int GameRunner::getAScore() const
//{
//	return _playerAttributes[PLAYER_A].score;
//}
//
//int GameRunner::getBScore() const
//{
//	return _playerAttributes[PLAYER_B].score;
//}
//
//bool GameRunner::didAWin() const
//{
//	return _playerAttributes[PLAYER_A].won;
//}
//
//bool GameRunner::didBWin() const
//{
//	return _playerAttributes[PLAYER_B].won;
//}
void GameRunner::handleMove(const MyBoardData& board, Coordinate& move, int &attackerNum, int &defenderNum, unique_ptr<IBattleshipGameAlgo>& A,
	unique_ptr<IBattleshipGameAlgo>& B, PlayerAttributes playerAttributesArr[])
{
	bool validAttack;
	// Remember moves are from 1 to ROW/COL SIZE while the board is from 0 to ROW/COL SIZE -1
	// hence we need to give a (-1) offset to the move coordinates
	auto hitChar = board.charAt(move);
	if (hitChar == CommonUtilities::WATER)
	{
		// Miss
		handleMiss(move, A, B, attackerNum);
	}
	else // Hit xor Sink xor double hit xor hit a sunken ship
	{
		handleHitOrSink(move, validAttack, A, B, hitChar, attackerNum, playerAttributesArr);
		// in case where there was a "real" hit (i.e a "living" tile got a hit) and it wasn't a self it, the attacker gets another turn
		if (validAttack && !(isupper(hitChar) ^ attackerNum))
		{
			return;
		}
	}
	//Change player
	changeAttacker(attackerNum, defenderNum);
}

void GameRunner::handleMiss(Coordinate& move, unique_ptr<IBattleshipGameAlgo>& A, unique_ptr<IBattleshipGameAlgo>& B, int& attackerNum)
{
	A->notifyOnAttackResult(attackerNum, move, AttackResult::Miss);
	B->notifyOnAttackResult(attackerNum, move, AttackResult::Miss);
}

void GameRunner::handleHitOrSink(Coordinate& move, bool& validAttack, unique_ptr<IBattleshipGameAlgo>& A, unique_ptr<IBattleshipGameAlgo>& B,
	char hitChar, int attackerNum, PlayerAttributes playerAttributesArr[])
{
	AttackResult attackResult;
	validAttack = registerHit(playerAttributesArr[(isupper(hitChar) ? 0 : 1)], move, CommonUtilities::charToShipType(hitChar), attackResult);
	//notify players on attack results
	A->notifyOnAttackResult(attackerNum, move, attackResult);
	B->notifyOnAttackResult(attackerNum, move, attackResult);
	if (attackResult == AttackResult::Sink)
	{
		// if hitChar is an UPPERCASE char - than A was hit and B gets the points (and vice versa)
		playerAttributesArr[(isupper(hitChar) ? 1 : 0)].score += calculateSinkScore(hitChar);
	}
}

int GameRunner::calculateSinkScore(char c)
{
	switch (toupper(c))
	{
	case CommonUtilities::BOAT:
		return BOAT_SCORE;
	case CommonUtilities::MISSLE_SHIP:
		return MISSLE_SHIP_SCORE;
	case CommonUtilities::SUBMARINE:
		return SUBMARINE_SCORE;
	case CommonUtilities::DESTROYER:
		return DESTROYER_SCORE;
	default:
		return -1000; // for debug
	}
}



bool GameRunner::registerHit(PlayerAttributes& playerAttributes, Coordinate coords, eShipType shipType, AttackResult& res)
{
	auto i = 0;
	auto validAttack = false;
	for (; i < playerAttributes.shipList.size(); i++)
	{
		if (playerAttributes.shipList[i].getType() == shipType)
		{
			//Make sure this coordinate belongs to this ship
			if (playerAttributes.shipList[i].getCoordinates().count(coords) == 1)
			{
				validAttack = playerAttributes.shipList[i].handleHit(coords, res);
				if (res == AttackResult::Sink)
				{
					playerAttributes.shipsCount--;

				}
				break;
			}
		}
	}
	return validAttack;
}

void GameRunner::initPlayersAttributes(const MyBoardData& board, const int player_id)
{
	_playerAttributes[player_id].hasMoves = true;
	_playerAttributes[player_id].score = 0;
	_playerAttributes[player_id].shipsCount = board.getShipCount();
	_playerAttributes[player_id].won = false;
	_playerAttributes[player_id].shipList = board.getShipList(player_id);

}