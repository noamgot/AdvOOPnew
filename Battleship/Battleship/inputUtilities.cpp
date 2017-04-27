#include "InputUtilities.h"
#include <direct.h>
#include <iostream>
#include <set>

#define MAX_PATH_LEN 1024
#define BUFF_SIZE 4096

using namespace std;

int searchFiles(const string dirPath, string& atkPathA, string& atkPathB, string& boardPath)
{
	string boardSuffix(".sboard");
	string aSuffix(".attack-a");
	string bSuffix(".attack-b");
	string sysDIR("2>NUL dir /a-d /b \"" + dirPath + "\"");
	string sysDIRpathChk("dir /a \"" + dirPath + "\"");
	string line;
	size_t lineSize;
	size_t pos;
	int errChk = 5;
	int errCount = 0;
	int ret = 0;
	char buffer[BUFF_SIZE];
	string fileName;
	
	// check for any errors in the directory path
	// TODO: Find better solution to check whether a folder is empty or has an invalid path
	FILE* errList = _popen(sysDIRpathChk.c_str(), "r");
	while (fgets(buffer, BUFF_SIZE - 1, errList))
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
	FILE* fileList = _popen(sysDIR.c_str(), "r");
	while (fgets(buffer, BUFF_SIZE - 1, fileList))
	{
		line = string(buffer);
		line.pop_back();

		lineSize = line.length();
		pos = line.rfind(boardSuffix);
		if ((boardPath == "") && (pos != -1) && (pos == lineSize - boardSuffix.length()))
		{
			boardPath = line;
			continue;
		}

		pos = line.rfind(aSuffix);
		if ((atkPathA == "") && (pos != -1) && (pos == lineSize - aSuffix.length()))
		{
			atkPathA = line;
			continue;
		}

		pos = line.rfind(bSuffix);
		if ((atkPathB == "") && (pos != -1) && (pos == lineSize - bSuffix.length()))
		{
			atkPathB = line;
			continue;
		}
	}
	_pclose(fileList);

	if (boardPath == "")
	{
		cout << "Missing board file (*.sboard) looking in path: " << dirPath << endl;
		ret = -1;
	}
	if (atkPathA == "")
	{
		cout << "Missing attack file for player A (*.attack-a) looking in path: " << dirPath << endl;
		ret = -1;
	}
	if (atkPathB == "")
	{
		cout << "Missing attack file for player B (*.attack-b) looking in path: " << dirPath << endl;
		ret = -1;
	}

	return ret;
}


string getDirPath()
{
	char *buff = nullptr;
	// TODO @Ben - I'm not sure that _getcwd is the right choice
	// and not sure if it's legal... consider changing it
	buff = _getcwd(buff, MAX_PATH_LEN);
	if (!buff)
	{
		return BAD_STRING; //signs the string is bad
	}
	string temp(buff);
	delete buff;
	return temp;
}

void initBoard(const string boardPath, string* board)
{
	ifstream boardFile(boardPath);
	if (!boardFile.is_open())
	{
		std::cout << "Error while trying to open board file" << std::endl;
		return;
	}
	char chars[9] = {' ','B','P','M','D','b','p','m','d'};
	set<char> charSet;
	charSet.insert(chars, chars+9);

	for (int i = 0; i < ROW_SIZE; i++)
	{
		getline(boardFile, board[i]);
		if (board[i].back() == '\r') board[i].back() = ' '; // handles '\r'
		if (board[i].length() > COL_SIZE)
		{                     // ignores extra characters in line
			board[i].erase(COL_SIZE, board[i].length()-COL_SIZE);
		}
		else if (board[i].length() < COL_SIZE)
		{                // appends extra '_' in case of a short line
			board[i].append(COL_SIZE-board[i].length(), ' ');
		}
		if (boardFile.eof() && i < ROW_SIZE-1)
		{                // inserts extra rows of '_' in case of missing lines
			i++;
			for (; i < ROW_SIZE; i++)
			{
				board[i] = string(COL_SIZE, ' ');
			}
		}
	}
	for (int i = 0; i < ROW_SIZE; i++)
	{                        // convert non-valid characters to spaces
		for (int j = 0; j < COL_SIZE; j++)
		{
			if (charSet.find(board[i][j]) == charSet.end()) board[i][j] = ' ';
		}
	}
	boardFile.close();
}

void initIndividualBoards(string *board, char **boardA, char **boardB)
{
	char c;
	for (int i = 0; i < ROW_SIZE; ++i)
	{
		for (int j = 0; j < COL_SIZE; ++j)
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

int checkShape(string* board, const int size, int i, int j)
{
	int verL = 1, horL = 1;
	// run horizontally, check above and below
	while (j+horL < COL_SIZE && board[i][j] == board[i][j+horL])
	{
		if ((i+1 < ROW_SIZE && board[i][j] == board[i+1][j+horL]) ||
			(i-1 >= 0 && board[i][j] == board[i-1][j+horL]))
		{
			return -1;
		}
		horL++;
	}
	// run vertically, check right and left
	while (i+verL < ROW_SIZE && board[i][j] == board[i+verL][j])
	{
		if ((j+1 < COL_SIZE && board[i][j] == board[i+verL][j+1]) ||
			(j-1 >= 0 && board[i][j] == board[i+verL][j-1]))
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

int checkBoardValidity(string* board)
{
	int shipCountA = 0, shipCountB = 0, isShipA = 0, isShipB = 0, adjCheck = 0, ret = 0;
	map<char,int> shipsA = {{'B',1},{'P',2},{'M',3},{'D',4}};
	map<char,int> shipsB = {{'b',1},{'p',2},{'m',3},{'d',4}};
	char shipMapA[4] = {'B','P','M','D'};
	char shipMapB[4] = {'b','p','m','d'};
	bitset<4> errShipsA;                                    // error flags for ship misshapes
	bitset<4> errShipsB;

	for (int i = 0; i < ROW_SIZE; i++)
	{
		for (int j = 0; j < COL_SIZE; j++)
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

	// Print possible errors
	for (int i = 0; i < 4; i++)
	{
		if (errShipsA[i])
		{
			cout << "Wrong size or shape for ship " << shipMapA[i] << " for player A" << endl;
			ret = -1;
		}
	}
	for (int i = 0; i < 4; i++)
	{
		if (errShipsB[i]) {
			cout << "Wrong size or shape for ship " << shipMapB[i] << " for player B" << endl;
			ret = -1;
		}
	}
	if (shipCountA > DEFAULT_SHIPS_COUNT)
	{
		cout << "Too many ships for Player A" << endl;
		ret = -1;
	}
	else if (shipCountA < DEFAULT_SHIPS_COUNT)
	{
		cout << "Too few ships for Player A" << endl;
		ret = -1;
	}
	if (shipCountB > DEFAULT_SHIPS_COUNT)
	{
		cout << "Too many ships for Player B" << endl;
		ret = -1;
	}
	else if (shipCountB < DEFAULT_SHIPS_COUNT)
	{
		cout << "Too few ships for Player B" << endl;
		ret = -1;
	}
	if (adjCheck)
	{
		cout << "Adjacent Ships on Board" << endl;
		ret = -1;
	}
	return ret;
}

void initAttack(const string atkPath, vector<pair<int,int>>& attacks)
{
	string line;
	char nextChr;
	int x,y;
	ifstream atkFile(atkPath);
	if (!atkFile.is_open())
	{
		std::cout << "Error while trying to open attack file" << std::endl;
		return;
	}

	while(getline(atkFile, line))
	{
		if (line == "")
		{
			continue;
		}
		if (line . back() == '\r')
		{
			line . back() = ' ';
		}
		x = -1;
		y = -1;
		stringstream lineStream(line);
		lineStream >> y;  //read y coor
		if (y < 1 || y > ROW_SIZE)
		{
			continue;
		}

		while (lineStream >> nextChr && nextChr == ' '){} //seek comma

		if (lineStream . eof() || nextChr != ',')
		{
			continue;
		}

		lineStream >> x;                                    //read x coor
		if (x < 1 || x > COL_SIZE)
		{
			continue;
		}

		attacks.push_back(make_pair(y,x));
	}
	atkFile.close();
}