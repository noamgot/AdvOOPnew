
#include "GameUtilities.h"
#include "Graphics.h"
#include <iostream>
#include "InputUtilities.h"

using namespace std;

#define BUF_SIZE 1024

// initialize static consts

const string GameUtilities::PARAM_QUIET("-quiet");
const string GameUtilities::PARAM_DELAY("-delay");
const string GameUtilities::BOARD_FILE_SUFFIX(".sboard");

int GameUtilities::processInputArguments(int argc, char** argv, bool& playWithGraphics, int& sleepTime,
										 string& dirPath, string& boardPath, string& atkPathA, string& atkPathB)
{
	
	auto gotDirPath = false;
	if (argc >= 2)
	{ // we accept the arguments in any order, and we assume that if a folder path is given it is the first argument
		for (auto i = 1; i < argc; ++i)
		{
			if (!strcmp(argv[i], PARAM_QUIET.c_str()))
			{
				playWithGraphics = false;
			}
			else if (!strcmp(argv[i], PARAM_DELAY.c_str()))
			{
				if (i + 1 < argc)
				{
					char *p;
					auto delay = strtol(argv[i + 1], &p, 10);
					if (!*p)
					{
						sleepTime = static_cast<DWORD>(delay);
					}
					// if there's no integer after PARAM_DELAY - we ignore this parameter...
				}
			}
			else if (i == 1)
			{ // we assume that if there's a folder path it is the first argument
				dirPath = argv[1];
				if (dirPath.front() == '\\' || dirPath.front() == '/') // if path is relative convert to absolute
				{
					dirPath = getDirPath() + dirPath;
				}
				//TODO - add the  following print + DLL search
				//cout << "Missing board file (*.sboard) looking in path: " << dirPath << endl;
				if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
				{
					return -1;
				}
				boardPath = dirPath + "/" + boardPath;
				atkPathA = dirPath + "/" + atkPathA;
				atkPathB = dirPath + "/" + atkPathB;
				gotDirPath = true;
			}
		}
	}
	if (!gotDirPath) // directory path given
	{
		dirPath = getDirPath();
		if (dirPath == BAD_STRING) //error occurred in getDirPath()
		{
			perror("Error");
			return -1;
		}
		if (searchFiles(dirPath, atkPathA, atkPathB, boardPath) < 0)
		{
			return -1;
		}
	}

	//success
	return 0;
}

eShipType GameUtilities::charToShipType(char c)
{
	switch (toupper(c))
	{
	case BOAT:
		return eShipType::SHIP_TYPE_B;
	case MISSLE_SHIP:
		return eShipType::SHIP_TYPE_P;
	case SUBMARINE:
		return eShipType::SHIP_TYPE_M;
	case DESTROYER:
		return eShipType::SHIP_TYPE_D;
	default:
		return eShipType::SHIP_TYPE_ERROR;
	}
}

bool GameUtilities::isIllegalMove(std::pair<int, int> move, int numOfRows, int numOfCols)
{
	auto x = move.first;
	auto y = move.second;
	return (x < 1 || x > numOfRows || y < 1 || y > numOfCols);
}

int GameUtilities::calculateSinkScore(char c)
{
	switch (toupper(c))
	{
	case BOAT:
		return BOAT_SCORE;
	case MISSLE_SHIP:
		return MISSLE_SHIP_SCORE;
	case SUBMARINE:
		return SUBMARINE_SCORE;
	case DESTROYER:
		return DESTROYER_SCORE;
	default:
		return -1;
	}
}

void GameUtilities::changeCurrentPlayer(int *attackerNum, int *defenderNum)
{
	*attackerNum = *attackerNum ? 0 : 1;
	*defenderNum = *defenderNum ? 0 : 1;
}


void GameUtilities::printGameResults(AbstractPlayer *pPlayers[], int scores[], bool playWithGraphics)
{	
	if (playWithGraphics)
	{
		Graphics::clearLastLine();
	}
	if (!pPlayers[0]->hasShips())
	{
		cout << "Player B won" << endl;
	}
	else if (!pPlayers[1]->hasShips())
	{
		cout << "Player A won" << endl;
	}
	cout << "Points:\nPlayer A:" << scores[0] << "\nPlayer B:" << scores[1] << endl;
}


char** GameUtilities::allocateBoard(int rows, int cols)
{
	auto board = new char *[rows];
	for (auto i = 0; i < rows; ++i)
	{
		board[i] = new char[cols];
	}
	return board;
}

void GameUtilities::deleteBoard(char** board, int rows)
{
	for (auto i = 0; i < rows; ++i)
	{
		delete[] board[i];
	}
	delete[] board;
}


int GameUtilities::initGameBoards(const string boardPath, string* board, char** boardA, char** boardB)
{
	ifstream boardFile(boardPath);
	if (!boardFile.is_open())
	{
		cout << "Error while trying to open board file" << endl;
		return -1;
	}
	char chars[9] = { ' ','B','P','M','D','b','p','m','d' };
	set<char> charSet;
	charSet.insert(chars, chars + 9);

	for (auto i = 0; i < ROW_SIZE; i++)
	{
		getline(boardFile, board[i]);
		if (board[i].size() > 0 && board[i].back() == '\r')
		{
			board[i].back() = ' '; // handles '\r'
		}
		if (board[i].length() > COL_SIZE)
		{                     // ignores extra characters in line
			board[i].erase(COL_SIZE, board[i].length() - COL_SIZE);
		}
		else if (board[i].length() < COL_SIZE)
		{                // appends extra '_' in case of a short line
			board[i].append(COL_SIZE - board[i].length(), ' ');
		}
		if (boardFile.eof() && i < ROW_SIZE - 1)
		{                // inserts extra rows of '_' in case of missing lines
			i++;
			for (; i < ROW_SIZE; i++)
			{
				board[i] = string(COL_SIZE, ' ');
			}
		}
	}
	for (auto i = 0; i < ROW_SIZE; i++)
	{                        // convert non-valid characters to spaces
		for (auto j = 0; j < COL_SIZE; j++)
		{
			if (charSet.find(board[i][j]) == charSet.end()) board[i][j] = ' ';
		}
	}
	boardFile.close();
	// validate board
	if (checkBoardValidity(board) < 0)
	{
		return -1;
	}
	initIndividualBoards(board, boardA, boardB);
	return 0;
}

void GameUtilities::initIndividualBoards(string *board, char **boardA, char **boardB)
{
	char c;
	for (auto i = 0; i < ROW_SIZE; ++i)
	{
		for (auto j = 0; j < COL_SIZE; ++j)
		{
			c = board[i][j];
			if (isalpha(c)) //part of a ship
			{
				if (isupper(c)) // a ship of A
				{
					boardA[i][j] = c;
					boardB[i][j] = WATER;
				}
				else // a ship of B
				{
					boardA[i][j] = WATER;
					boardB[i][j] = c;
				}
			}
			else // a space - update both boards
			{
				boardA[i][j] = WATER;
				boardB[i][j] = WATER;
			}
		}
	}
}

int GameUtilities::checkShape(string* board, const int size, int i, int j)
{
	auto verL = 1, horL = 1;
	// run horizontally, check above and below
	while (j + horL < COL_SIZE && board[i][j] == board[i][j + horL])
	{
		if ((i + 1 < ROW_SIZE && board[i][j] == board[i + 1][j + horL]) ||
			(i - 1 >= 0 && board[i][j] == board[i - 1][j + horL]))
		{
			return -1;
		}
		horL++;
	}
	// run vertically, check right and left
	while (i + verL < ROW_SIZE && board[i][j] == board[i + verL][j])
	{
		if ((j + 1 < COL_SIZE && board[i][j] == board[i + verL][j + 1]) ||
			(j - 1 >= 0 && board[i][j] == board[i + verL][j - 1]))
		{
			return -1;
		}
		verL++;
	}
	// check for misshape in size
	if ((horL > 1 && verL > 1) || (horL != size && verL != size))
	{
		return -1;
	}
	return 1;
}

int GameUtilities::printBoardErrors(bitset<4>& errShipsA, bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck)
{
	auto ret = 0;
	string filler;
	char shipMap[4] = { BOAT,MISSLE_SHIP,SUBMARINE,DESTROYER};
	// Print possible errors
	for (auto i = 0; i < 4; i++)
	{
		if (errShipsA[i])
		{
			cout << "Wrong size or shape for ship " << shipMap[i] << " for player A" << endl;
			ret = -1;
		}
	}
	for (auto i = 0; i < 4; i++)
	{
		if (errShipsB[i]) {
			cout << "Wrong size or shape for ship " << tolower(shipMap[i]) << " for player B" << endl;
			ret = -1;
		}
	}
	if (shipCountA != DEFAULT_SHIPS_COUNT)
	{
		filler = (shipCountA > DEFAULT_SHIPS_COUNT) ? "many" : "few";
		cout << "Too " << filler << " ships for Player A" << endl;
		ret = -1;
		/*if (shipCountA > DEFAULT_SHIPS_COUNT)
		{
			cout << "Too many ships for Player A" << endl;
			ret = -1;
		}
		else if (shipCountA < DEFAULT_SHIPS_COUNT)
		{
			cout << "Too few ships for Player A" << endl;
			ret = -1;
		}*/
	}
	if (shipCountB != DEFAULT_SHIPS_COUNT)
	{
		filler = (shipCountB > DEFAULT_SHIPS_COUNT) ? "many" : "few";
		cout << "Too " << filler << " ships for Player B" << endl;
		ret = -1;
	}
	/*if (shipCountB > DEFAULT_SHIPS_COUNT)
	{
		cout << "Too many ships for Player B" << endl;
		ret = -1;
	}
	else if (shipCountB < DEFAULT_SHIPS_COUNT)
	{
		cout << "Too few ships for Player B" << endl;
		ret = -1;
	}*/
	if (adjCheck)
	{
		cout << "Adjacent Ships on Board" << endl;
		ret = -1;
	}
	return ret;
}

int GameUtilities::checkBoardValidity(string* board)
{
	auto shipCountA = 0, shipCountB = 0, isShipA = 0, isShipB = 0, adjCheck = 0;
	map<char, int> shipsA = { { 'B',1 },{ 'P',2 },{ 'M',3 },{ 'D',4 } };
	map<char, int> shipsB = { { 'b',1 },{ 'p',2 },{ 'm',3 },{ 'd',4 } };
	bitset<4> errShipsA;                                    // error flags for ship misshapes
	bitset<4> errShipsB;

	for (auto i = 0; i < ROW_SIZE; i++)
	{
		for (auto j = 0; j < COL_SIZE; j++)
		{
			if (board[i][j] != ' ')
			{
				isShipA = (shipsA[board[i][j]] != 0);       // 1 if its A ship, otherwise 0
				isShipB = (shipsB[board[i][j]] != 0);
				if (isShipA || isShipB)
				{
					// check if its new
					if (!((i != 0 && board[i - 1][j] == board[i][j]) ||
						(j != 0 && board[i][j - 1] == board[i][j])))
					{
						// check for misshape
						if (checkShape(board, shipsB[tolower(board[i][j])], i, j) < 0)
						{
							if (isShipA)
							{
								errShipsA[shipsA[board[i][j]] - 1] = 1;
							}
							if (isShipB)
							{
								errShipsB[shipsB[board[i][j]] - 1] = 1;
							}
						}
						else
						{
							shipCountA += isShipA;
							shipCountB += isShipB;
						}
					}
					// Check if any adjacent ships exist
					if (((j != 0) && (board[i][j - 1] != board[i][j]) && (board[i][j - 1] != ' ')) ||
						((i != 0) && (board[i - 1][j] != board[i][j]) && (board[i - 1][j] != ' ')))
					{
						adjCheck = 1;
					}
				}
			}
		}
	}

	return printBoardErrors(errShipsA, errShipsB, shipCountA, shipCountB, adjCheck);
}

int GameUtilities::findFileBySuffix(string& filePath, const string dirPath, const string suffix)
{
	string line;
	auto sysDIR("2>NUL dir /a-d /b \"" + dirPath + "\""), sysDIRpathChk("dir /a \"" + dirPath + "\"");
	size_t pos;
	auto errChk = 5, errCount = 0;
	char buffer[BUF_SIZE];
	string fileName;

	// check for any errors in the directory path
	// TODO: Find better solution to check whether a folder is empty or has an invalid path
	auto errList = _popen(sysDIRpathChk.c_str(), "r");
	while (fgets(buffer, BUF_SIZE - 1, errList))
	{
		errCount++;
	}
	if (errCount < errChk)
	{
		cout << "Wrong Path " << dirPath << endl;
		_pclose(errList);
		return -1;
	}
	_pclose(errList);

	// parse directory contents
	//TODO @Ben - this part can be more efficient - need to break when all paths are found
	auto fileList = _popen(sysDIR.c_str(), "r");
	while (fgets(buffer, BUF_SIZE - 1, fileList))
	{
		line = string(buffer);
		line.pop_back();

		pos = line.rfind(suffix);
		if (filePath == "" && pos != string::npos && pos == line.length() - suffix.length())
		{
			filePath = line;
		}
	}
	_pclose(fileList);

	return (filePath == "") ? -1 : 0;
}


