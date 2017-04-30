#include "FilePlayer.h"


const std::string FilePlayer::ATTACK_FILE_SUFFIX(".attack");

FilePlayer::FilePlayer()
{
}


FilePlayer::~FilePlayer()
{
}

void FilePlayer::setMoves(std::vector<std::pair<int, int>> moves)
{
	for (size_t i = 0; i < moves.size(); ++i)
	{
		// we assume that if we got here all the moves are valid
		auto move = std::make_pair(moves[i].first, moves[i].second);
		this->mMovesQueue.push(move);
	}
}

//void FilePlayer::setBoard(int player, const char ** board, int numRows, int numCols) {}

bool FilePlayer::init(const std::string & path)
{
	AbstractPlayer::init(path);
	return false;
}
