
#include "Utilities.h"
#include <iostream>
#include <filesystem>
#include <Windows.h>

using namespace std;

namespace Utilities
{
	eShipType Utilities::charToShipType(char c)
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


	char** Utilities::allocateBoard(int rows, int cols)
	{
		auto board = new char *[rows];
		for (auto i = 0; i < rows; ++i)
		{
			board[i] = new char[cols];
		}
		return board;
	}

	void Utilities::deleteBoard(char** board, int rows)
	{
		for (auto i = 0; i < rows; ++i)
		{
			delete[] board[i];
		}
		delete[] board;
	}


	void Utilities::filterAndSortFileList(const vector<string>& fileList, vector<string>& filteredFileList, const string suffix)
	{
		// copy all relevant files to the filteredFileList vector
		for (auto i = 0; i < fileList.size(); i++)
		{
			if (endsWith(fileList[i], suffix))
			{
				filteredFileList.push_back(fileList[i]);
			}

		}
		// sort the filtered file list
		sort(filteredFileList.begin(), filteredFileList.end());
	}

	int Utilities::findFileBySuffix(string& filePath, const string dirPath, const string suffix,
	                                    bool& fileNotFound, int playerNum, bool allowSingleFile)
	{
		// for debug; should not get here
		if (playerNum > 1 || playerNum < 0)
		{
			cout << "Error: invalid player number" << endl;
			return -1;
		}
		// make sure we get a clear filePath string
		filePath.clear();
		vector<string> fileList, filteredFileList;
		if (getDirectoryFileList(dirPath, fileList) < 0)
		{
			return -1;
		}
		filterAndSortFileList(fileList, filteredFileList, suffix);
		// we're optimistic - we believe there is a file (we might change our opinion soon :) )
		fileNotFound = false;
		switch (filteredFileList.size())
		{
		case 0: // no files
			fileNotFound = true;
			break;
		case 1: // single file
			if (allowSingleFile) // board or attack files
			{
				filePath = filteredFileList[0]; // take the only file
				break;
			}
			// if it is a dll file we do not allow a single file - hence we report that a file was not found
			fileNotFound = true; 
			break;
		default: // 2 files or more - take the first or second file according to player number
			filePath = filteredFileList[playerNum];
		}
		return fileNotFound ? -1 : 0;
	}


	bool Utilities::isValidPath(const string dirPath)
	{
		auto dwAttrib = GetFileAttributesA(dirPath.c_str());
		return dwAttrib != INVALID_FILE_ATTRIBUTES &&
			dwAttrib & FILE_ATTRIBUTE_DIRECTORY;
	}


	int Utilities::getDirectoryFileList(const string dirPath, vector<string>& fileListVector)
	{
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
			string line(buffer);
			line.pop_back(); // clear new line character
			fileListVector.push_back(line);
		}
		_pclose(fileList);
		return 0;
	}

	bool Utilities::endsWith(const string str, const string suffix)
	{
		auto pos = str.rfind(suffix);
		return pos != string::npos && pos == str.length() - suffix.length();
	}

	int getPathByType(string& filePath, const string dirPath, const string fileSuffix, eFileType fileType, int playerNum)
	{
		auto fileNotFound = true;
		// we allow finfing 2 files only in case of attack files
		if (findFileBySuffix(filePath, dirPath, fileSuffix, fileNotFound, 
				playerNum, fileType != eFileType::DLL_FILE) < 0) // we do not allow a single file for dll files
		{
			if (fileNotFound)
			{
				switch (fileType)
				{
					case eFileType::BOARD_FILE:
						cout << "Missing board file (*.sboard) looking in path: " << dirPath << endl;
						break;
					case eFileType::DLL_FILE:
						cout << "Missing an algorithm (dll) file looking in path: " << dirPath << endl;
						break;
					default:
						break;
				}
			}
			return -1;
		}
		// convert filePath to its full path
		filePath = dirPath + "\\" + filePath;
		return 0;
	}
}