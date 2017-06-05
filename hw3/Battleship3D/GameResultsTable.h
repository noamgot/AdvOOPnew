#pragma once
#include "CommonUtilities.h"
#include "PlayerGameResults.h"


// todo - consider being friend class with CompetitionManager

class GameResultsTable
{
public:
	GameResultsTable() = default;
	GameResultsTable(size_t numPlayers, size_t numRounds) : _numPlayers(numPlayers), _table(numRounds){}
	

	// block copy & move ctors and assignments
	GameResultsTable(const GameResultsTable& other) = delete;
	GameResultsTable& operator=(const GameResultsTable& other) = delete;
	GameResultsTable(GameResultsTable&& other) noexcept = delete;
	GameResultsTable& operator=(GameResultsTable&& other) noexcept = delete;

	/* the wanted round results*/
	const std::vector<PlayerGameResults>& getRoundResults(int round);
	void updateTable(int round, PlayerGameResults gameResults);
	void notifyEndGame() { _cv.notify_one(); }

private:
	std::vector<std::vector<PlayerGameResults>> _table;
	std::mutex _mutex;
	std::condition_variable _cv;
	size_t _numPlayers;
};
