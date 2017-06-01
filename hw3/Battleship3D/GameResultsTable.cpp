#include "GameResultsTable.h"


const std::vector<PlayerGameResults>& GameResultsTable::getRoundResults(int round)
{
	std::unique_lock<std::mutex> mlock(_mutex);
	_cv.wait(mlock, [this, round] {return _table[round].size() == _numPlayers; });
	return _table[round]; 
}

void GameResultsTable::updateTable(int round, PlayerGameResults gameResults)
{
	std::unique_lock<std::mutex> mlock(_mutex);
	_table[round].push_back(gameResults);
	mlock.unlock();
	_cv.notify_one();

}
