#pragma once
#include <condition_variable>
#include <vector>
#include "BattleshipTournament.h"

// todo - consider being friend class with BattleshipTournament

class GameResultsTable
{
public:
	GameResultsTable() = delete;
	GameResultsTable(int numPlayers, int numRounds) : _numPlayers(numPlayers), _numRounds(numRounds) { _table.resize(numRounds); }
	

	// block copy & move ctors and assignments
	GameResultsTable(const GameResultsTable& other) = delete;
	GameResultsTable& operator=(const GameResultsTable& other) = delete;
	GameResultsTable(GameResultsTable&& other) noexcept = delete;
	GameResultsTable& operator=(GameResultsTable&& other) noexcept = delete;

	/* the wanted round results*/
	const std::vector<PlayerGameResults>& getRoundResults(int round);
	void updateTable(int round, PlayerGameResults gameResults);

private:
	std::vector<std::vector<PlayerGameResults>> _table;
	std::mutex _mutex;
	std::condition_variable _cv;
	int _numPlayers;
	int _numRounds;
};
