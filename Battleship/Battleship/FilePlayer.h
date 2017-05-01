#pragma once
#include "AbstractPlayer.h"

class FilePlayer :	public AbstractPlayer
{

public:
	FilePlayer();
	~FilePlayer();

	void setBoard(int player, const char **board, int numRows, int numCols) override; // override with an empty implementation
	bool init(const string& path) override;

private:
	static const string ATTACK_FILE_SUFFIX;
	int loadMovesFromFile(const string attackFilePath);
};

