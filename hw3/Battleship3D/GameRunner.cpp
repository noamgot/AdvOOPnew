﻿#include "GameRunner.h"

using namespace std;
using namespace CommonUtilities;

GameRunner::GameRunner(const CompetitionManager::Game& game, const GetAlgoFuncType& getAlgoA, const GetAlgoFuncType& getAlgoB, 
					const MyBoardData& boardData, const MyBoardData& boardA, const MyBoardData& boardB, shared_ptr<Logger> pLogger)
	:_playerA(getAlgoA()), _playerB(getAlgoB()), _boardData(boardData), _boardA(boardA), _boardB(boardB), _pLogger(pLogger), _game(game),
	 _attackerNum(0), _defenderNum(1)
{
	initPlayersAttributes(PLAYER_A);
	initPlayersAttributes(PLAYER_B);
	_pLogger->writeToLog("Initialized game: boardID=" + to_string(game._boardID) + ", A=" + to_string(game._idA) + ", B=" + to_string(game._idB));
}

string GameRunner::winnerConclusionStr() const
{
	if (_grA.wins)
	{
		return "Player A won";
	}
	if (_grB.wins)
	{
		return "Player B won";
	}
	return "Tie";
}

void GameRunner::processGameResults(int movesCnt, int maxMoves)
{
	_grA = PlayerGameResults(_game._idA, 0, 0, _playerAttributes[PLAYER_A].score, _playerAttributes[PLAYER_B].score, 0);
	_grB = PlayerGameResults(_game._idB, 0, 0, _playerAttributes[PLAYER_B].score, _playerAttributes[PLAYER_A].score, 0);
	if (_playerAttributes[PLAYER_A].shipsCount <= 0)
	{
		_grB.wins = _grA.losses = 1;
		_grB.percentage = 100;
	}
	else if (_playerAttributes[PLAYER_B].shipsCount <= 0)
	{
		_grA.wins = _grB.losses = 1;
		_grA.percentage = 100;
	}
	_pLogger->writeToLog("Finished game: boardID=" + to_string(_game._boardID) + ", A=" + to_string(_game._idA) + ", B=" + to_string(_game._idB) +
		" $$$ Results: A-" + to_string(_grA.ptsFor) + ", B-" + to_string(_grB.ptsFor) + " - " + winnerConclusionStr());
	if(movesCnt > maxMoves)
	{
		_pLogger->writeToLog("Game was terminated due to very high moves count (only " + 
								to_string(maxMoves) + " allowed", false, Logger::eLogType::LOG_WARNING);
	}
}

void GameRunner::runGame()
{
	IBattleshipGameAlgo *pPlayers[] = { _playerA.get() , _playerB.get() };
	initPlayer(_playerA.get(), PLAYER_A, _boardA);
	initPlayer(_playerB.get(), PLAYER_B, _boardB);

	// in order not to get stuck, we limit the amount of moves for a single game with a very generous maximal moves amount
	auto maxMoves = _boardData.rows() * _boardData.cols() * _boardData.depth() * 10;
	auto movesCnt = 0;
	//The game goes on until one of the players has no more ships or both ran out of moves.
	while (_playerAttributes[0].shipsCount > 0 && _playerAttributes[1].shipsCount > 0 &&
		(_playerAttributes[0].hasMoves || _playerAttributes[1].hasMoves) && movesCnt <= maxMoves)
	{
		// Skip if current player is out of moves.
		if (!_playerAttributes[_attackerNum].hasMoves)
		{
			changeAttacker();
			continue;
		}

		auto currentMove = pPlayers[_attackerNum]->attack();
		movesCnt++; // we increment movesCnt only if a move is played
		//_pLogger->writeToLog("Attacker: " + to_string(_attackerNum) + "; Got move: (" + to_string(currentMove.row) + "," + to_string(currentMove.col) + "," + to_string(currentMove.depth) + ")");
		if (!isLegalMove(currentMove, _boardData.rows(), _boardData.cols(), _boardData.depth()))
		{
			// Player returned a dumb move or refuses to play like a big cry baby (or maybe it has a bug)
			// we switch player and continue - even if the move is invalid (i.e we ignore invalid moves)
			_playerAttributes[_attackerNum].hasMoves = !(currentMove.row == -1 && currentMove.col == -1 && currentMove.depth == -1);
			changeAttacker();
			continue;
		}
		handleMove(currentMove);
	}
	processGameResults(movesCnt,maxMoves);
}

void GameRunner::handleMove(Coordinate& move)
{
	bool validAttack;
	auto hitChar = _boardData.charAt(move);
	if (hitChar == WATER)
	{
		// Miss
		handleMiss(move);
	}
	else // Hit xor Sink xor double hit xor hit a sunken ship
	{
		handleHitOrSink(move, validAttack, hitChar);
		// in case where there was a "real" hit (i.e a "living" tile got a hit) and it wasn't a self it, the attacker gets another turn
		if (validAttack && !(isupper(hitChar) ^ _attackerNum))
		{
			return;
		}
	}

	changeAttacker();
}

void GameRunner::handleMiss(Coordinate& move) const
{
	_playerA->notifyOnAttackResult(_attackerNum, move, AttackResult::Miss);
	_playerB->notifyOnAttackResult(_attackerNum, move, AttackResult::Miss);
}

void GameRunner::handleHitOrSink(Coordinate& move, bool& validAttack, char hitChar)
{
	AttackResult attackResult;
	validAttack = registerHit(isupper(hitChar) ? 0 : 1, move, charToShipType(hitChar), attackResult);
	//notify players on attack results
	_playerA->notifyOnAttackResult(_attackerNum, move, attackResult);
	_playerB->notifyOnAttackResult(_attackerNum, move, attackResult);
	if (attackResult == AttackResult::Sink)
	{
		// if hitChar is an UPPERCASE char - than A was hit and B gets the points (and vice versa)
		_playerAttributes[(isupper(hitChar) ? 1 : 0)].score += calculateSinkScore(hitChar);
	}
}

int GameRunner::calculateSinkScore(char c)
{
	switch (toupper(c))
	{
	case BOAT:
		return Ship::eShipScore::BOAT_SCORE;
	case MISSLE_SHIP:
		return Ship::eShipScore::MISSLE_SHIP_SCORE;
	case SUBMARINE:
		return Ship::eShipScore::SUBMARINE_SCORE;
	case DESTROYER:
		return Ship::eShipScore::DESTROYER_SCORE;
	default:
		return -10000; // for debug
	}
}



bool GameRunner::registerHit(int hitPlayer, Coordinate coords, Ship::eShipType shipType, AttackResult& res)
{
	auto validAttack = false;
	for (auto i = 0 ; i < _playerAttributes[hitPlayer].shipList.size(); i++)
	{
		if (_playerAttributes[hitPlayer].shipList[i].getType() == shipType)
		{
			//Make sure this coordinate belongs to this ship
			auto coordinates = _playerAttributes[hitPlayer].shipList[i].getCoordinates();
			if (coordinates.count(coords) == 1)
			{
				validAttack = _playerAttributes[hitPlayer].shipList[i].handleHit(coords, res);
				if (res == AttackResult::Sink)
				{
					_playerAttributes[hitPlayer].shipsCount--;
				}
				break;
			}
		}
	}
	return validAttack;
}

void GameRunner::initPlayersAttributes(const int player_id)
{
	_playerAttributes[player_id].hasMoves = true;
	_playerAttributes[player_id].score = 0;
	_playerAttributes[player_id].shipsCount = _boardData.getShipCount(player_id);
	_playerAttributes[player_id].shipList = _boardData.getShipList(player_id);
	//_playerAttributes[player_id].movesCnt = 0;

}
