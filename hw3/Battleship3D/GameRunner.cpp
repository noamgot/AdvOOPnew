#include "GameRunner.h"

using namespace std;

GameRunner::GameRunner(const CommonUtilities::Game& game, const GetAlgoFuncType& getAlgoA, const GetAlgoFuncType& getAlgoB, const MyBoardData& boardData, const MyBoardData& boardA, const MyBoardData& boardB, Logger* pLogger)
	:_playerA(getAlgoA()), _playerB(getAlgoB()), _boardData(boardData), _boardA(boardA), _boardB(boardB), _pLogger(pLogger), _game(game)
{
	initPlayersAttributes(boardData, PLAYER_A);
	initPlayersAttributes(boardData, PLAYER_B);
	_pLogger->writeToLog("Initialized game: boardID = " + to_string(game._boardID) + ", A=" + to_string(game._idA) + ", B: " + to_string(game._idB));
}

vector<PlayerGameResults> GameRunner::runGame()
{
	// Copy board to personal boards
	vector<MyBoardData> mbd(2);
	mbd[0] = MyBoardData(_boardData.rows(), _boardData.cols(), _boardData.depth());
	mbd[1] = MyBoardData(_boardData.rows(), _boardData.cols(), _boardData.depth());

	//initIndividualBoards(mbd[PLAYER_A], mbd[PLAYER_B]);

	//todo - something is WRONG here...

	//todo - make attacker and defender data members
	auto attackerNum = 0, defenderNum = 1; // index 0 = A, index 1 = B
	IBattleshipGameAlgo *pPlayers[] = { _playerA.get() , _playerB.get() };
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
void GameRunner::handleMove(const MyBoardData& board, Coordinate& move, int &attackerNum, int &defenderNum, IBattleshipGameAlgo* A,
							IBattleshipGameAlgo* B, PlayerAttributes playerAttributesArr[])
{
	bool validAttack;
	auto hitChar = board.charAt(move);
	if (hitChar == CommonUtilities::WATER)
	{
		// Miss
		handleMiss(move, attackerNum);
	}
	else // Hit xor Sink xor double hit xor hit a sunken ship
	{
		handleHitOrSink(move, validAttack, hitChar, attackerNum, playerAttributesArr);
		// in case where there was a "real" hit (i.e a "living" tile got a hit) and it wasn't a self it, the attacker gets another turn
		if (validAttack && !(isupper(hitChar) ^ attackerNum))
		{
			return;
		}
	}
	//Change player
	changeAttacker(attackerNum, defenderNum);
}

void GameRunner::handleMiss(Coordinate& move, int attackerNum) const
{
	_playerA->notifyOnAttackResult(attackerNum, move, AttackResult::Miss);
	_playerB->notifyOnAttackResult(attackerNum, move, AttackResult::Miss);
}

void GameRunner::handleHitOrSink(Coordinate& move, bool& validAttack,
								 char hitChar, int attackerNum, PlayerAttributes playerAttributesArr[])
{
	AttackResult attackResult;
	validAttack = registerHit(playerAttributesArr[(isupper(hitChar) ? 0 : 1)], move, CommonUtilities::charToShipType(hitChar), attackResult);
	//notify players on attack results
	_playerA->notifyOnAttackResult(attackerNum, move, attackResult);
	_playerB->notifyOnAttackResult(attackerNum, move, attackResult);
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