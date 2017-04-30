#pragma once
#include "AbstractPlayer.h"

class FilePlayer :	public AbstractPlayer
{
	static const std::string ATTACK_FILE_SUFFIX;

	int loadMovesFromFile(const std::string attackFilePath);

public:
	FilePlayer();
	~FilePlayer();
	//TODO @Noam - for now we'll not override this method. however, we may want to override it 
	//with an empty imp. in that case we'll have to change the imp. of initShipLists (which counts on the object's copy
	// of the board).
	//void setBoard(int player, const char **board, int numRows, int numCols) override; // override with an empty implementation
	bool init(const std::string& path) override;
};

