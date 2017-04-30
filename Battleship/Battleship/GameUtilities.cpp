
#include "GameUtilities.h"
#include "Graphics.h"
#include <iostream>
#include "InputUtilities.h"
#include <filesystem>

using namespace std;

#define BUF_SIZE 1024

// initialize static consts

const string GameUtilities::PARAM_QUIET("-quiet");
const string GameUtilities::PARAM_DELAY("-delay");
const string GameUtilities::BOARD_FILE_SUFFIX(".sboard");
const string GameUtilities::ATTACK_FILE_SUFFIX(".attack");
const string GameUtilities::BAD_STRING("!@#"); // for getDirPath
const int GameUtilities::DEFAULT_SHIPS_COUNT = 5;
const int GameUtilities::MAX_PATH_LEN = 1024;

void GameUtilities::processInputArguments(int argc, char** argv, bool& playWithGraphics, int& sleepTime, string& dirPath)
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
				gotDirPath = true;
			}
		}
	}
	if (!gotDirPath) // directory path not given
	{
		dirPath = getDirPath();
	}
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

bool GameUtilities::isLegalMove(pair<int, int> move, int numOfRows, int numOfCols)
{
	auto x = move.first;
	auto y = move.second;
	return !(x < 1 || x > numOfRows || y < 1 || y > numOfCols);
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

void GameUtilities::changeCurrentPlayer(int& attackerNum, int& defenderNum)
{
	attackerNum = attackerNum ? 0 : 1;
	defenderNum = defenderNum ? 0 : 1;
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


int GameUtilities::initGameBoards(const string boardPath, string board[], char** boardA, char** boardB)
{
	ifstream boardFile(boardPath);
	if (!boardFile.is_open())
	{
		cout << "Error: opening board file failed" << endl;
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
	}
	if (shipCountB != DEFAULT_SHIPS_COUNT)
	{
		filler = (shipCountB > DEFAULT_SHIPS_COUNT) ? "many" : "few";
		cout << "Too " << filler << " ships for Player B" << endl;
		ret = -1;
	}
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
	bitset<4> errShipsA, errShipsB;// error flags for ship misshapes

	for (auto i = 0; i < ROW_SIZE; i++)
	{
		for (auto j = 0; j < COL_SIZE; j++)
		{
			if (board[i][j] != ' ')
			{
				isShipA = shipsA[board[i][j]] != 0;       // 1 if its A ship, otherwise 0
				isShipB = shipsB[board[i][j]] != 0;
				if (isShipA || isShipB)
				{
					// check if its new
					if (!(i != 0 && board[i - 1][j] == board[i][j] ||
						j != 0 && board[i][j - 1] == board[i][j]))
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
					if (j != 0 && board[i][j - 1] != board[i][j] && board[i][j - 1] != ' ' ||
						i != 0 && board[i - 1][j] != board[i][j] && board[i - 1][j] != ' ')
					{
						adjCheck = 1;
					}
				}
			}
		}
	}

	return printBoardErrors(errShipsA, errShipsB, shipCountA, shipCountB, adjCheck);
}

int GameUtilities::getBoardPath(string& dirPath, string& boardPath)
{
	if (dirPath == BAD_STRING) //error occurred in getDirPath()
	{
		perror("Error");
		return -1;
	}
	// get the full path of the directory
	auto fullPath = _fullpath(nullptr, dirPath.c_str(), MAX_PATH_LEN);
	// copy the full path to dirPath and delete fullPath
	dirPath = fullPath;
	delete fullPath;
	// clear "\\" if added by _fullpath()
	if (dirPath.back() == '\\')
	{
		dirPath.pop_back();
	}
	//TODO - add DLL search
	if (!isValidPath(dirPath))
	{
		cout << "Wrong Path: " << dirPath << endl;
		return -1;
	}
	auto fileNotFound = true;
	if (findFileBySuffix(boardPath, dirPath, BOARD_FILE_SUFFIX, fileNotFound,0) < 0)
	{
		if (fileNotFound)
		{
			cout << "Missing board file (*" << BOARD_FILE_SUFFIX << ") looking in path: " << dirPath << endl;
		}
		return -1;
	}
	// convert boardPath to its full path
	boardPath = dirPath + "\\" + boardPath;
	return 0;
}


int getDirectorySortedFileList(const string dirPath, vector<string>& fileListVector)
{
	string line;
	fileListVector.clear();
	auto sysDIR("2>NUL dir /a-d /b \"" + dirPath + "\"");
	char buffer[BUF_SIZE];
	// parse directory and find the wanted file
	auto fileList = _popen(sysDIR.c_str(), "r");
	if(!fileList)
	{
		perror("Error");
		return -1;
	}
	while (fgets(buffer, BUF_SIZE - 1, fileList))
	{
		line = string(buffer);
		line.pop_back(); // clear new line character
		fileListVector.push_back(line);
	}
	_pclose(fileList);

	return 0;
}



int GameUtilities::findFileBySuffix(string& filePath, const string dirPath, const string suffix, 
										bool& fileNotFound, int playerNum)
{
	// for debug; should not get here
	if (playerNum > 1)
	{
		cout << "Error: invalid player number" << endl;
		return -1;
	}
	// make sure we get a clear filePath string
	filePath.clear();
	vector<string> fileListVector;
	if (getDirectorySortedFileList(dirPath, fileListVector) < 0)
	{
		return -1;
	}
	for (auto file : fileListVector)
	{
		if (endsWith(file, suffix))
		{
			filePath = file;
			fileNotFound = false;
			// the following section allows player no. 1 to find another instance
			// of the wanted file. player no. 0 gets the first file he finds
			if (--playerNum < 0)
			{
				break;
			}
		}
	}
	return fileNotFound ? -1 : 0;
}

string GameUtilities::getDirPath()
{
	char *buff = nullptr;
	buff = _getcwd(buff, MAX_PATH_LEN);
	if (!buff)
	{
		return BAD_STRING; //signs the string is bad
	}
	string temp(buff);
	delete buff;
	return temp;
}

bool GameUtilities::isValidPath(const string dirPath)
{
	auto dwAttrib = GetFileAttributesA(dirPath.c_str());
	return dwAttrib != INVALID_FILE_ATTRIBUTES &&
		   dwAttrib & FILE_ATTRIBUTE_DIRECTORY;
}

int GameUtilities::getDirectorySortedFileList(const string dirPath, vector<string>& fileListVector)
{
	string line;
	fileListVector.clear();
	auto sysDIR("2>NUL dir /a-d /b \"" + dirPath + "\"");
	char buffer[BUF_SIZE];
	// parse directory and find the wanted file
	auto fileList = _popen(sysDIR.c_str(), "r");
	if (!fileList)
	{
		perror("Error");
		return -1;
	}
	while (fgets(buffer, BUF_SIZE - 1, fileList))
	{
		line = string(buffer);
		line.pop_back(); // clear new line character
		fileListVector.push_back(line);
	}
	_pclose(fileList);
	//sort vector
	sort(fileListVector.begin(), fileListVector.end());

	return 0;
}

bool GameUtilities::endsWith(const string line, const string suffix)
{
	auto pos = line.rfind(suffix);
	return pos != string::npos && pos == line.length() - suffix.length();
}


