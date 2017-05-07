
#include "GameUtilities.h"
#include <iostream>
#include <filesystem>


using namespace std;

// initialize static consts
const int GameUtilities::DEFAULT_SHIPS_COUNT = 5;
const int GameUtilities::BUF_SIZE = 1024;


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


int GameUtilities::findFileBySuffix(string& filePath, const string dirPath, const string suffix, 
										bool& fileNotFound, int playerNum)
{
	// for debug; should not get here
	if (playerNum > 1 || playerNum < 0)
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
			// the following section allows player no. 1 to find another instance
			// of the wanted file. player no. 0 gets the first file he finds
			if (--playerNum < 0)
			{
				filePath = file;
				fileNotFound = false;
				break;
			}
		}
	}
	return fileNotFound ? -1 : 0;
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
