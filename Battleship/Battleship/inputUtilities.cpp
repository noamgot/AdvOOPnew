#include "InputUtilities.h"
#include "GameUtilities.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

#define ROW_SIZE 10
#define COL_SIZE 10

int searchFilesN(const string dirPath, string& dllPathA, string& dllPathB, string& boardPath)
{
	WIN32_FIND_DATAA fileData;
	
	string dllSuffix = "*.dll";
	string boardSuffix = "*.sboard";
	string dirBack = (dirPath.back() == '\\' || dirPath.back() == '/') ? "" : "\\";
	auto path = dirPath + dirBack;

	// find first sboard file
	auto dir = FindFirstFileA((path + boardSuffix).c_str(), &fileData);
	if (dir != INVALID_HANDLE_VALUE) // check if file successfully opened
	{
		boardPath = path + fileData.cFileName;
	}
	else
	{
		if (GetLastError() != ERROR_FILE_NOT_FOUND)	// check if directory path is valid
		{
			cout << "Wrong Path: " << dirPath << endl;
			return -1;
		}
		cout << "Missing board file (*.sboard) looking in path: " << dirPath << endl;
	}

	// find 2 dll files
	dir = FindFirstFileA((path + dllSuffix).c_str(), &fileData);
	if (dir == INVALID_HANDLE_VALUE)
	{
		cout << "Missing an algorithm(dll) file looking in path: " << dirPath << endl;
		return -1;
	}
	dllPathA = path + fileData.cFileName;

	FindNextFileA(dir, &fileData);
	if (dir == INVALID_HANDLE_VALUE)
	{
		cout << "Missing an algorithm(dll) file looking in path: " << dirPath << endl;
		return -1;
	}
	dllPathB = path + fileData.cFileName;

	return 1;
}

int initAttackNew(const string dirPath, queue<pair<int, int>>& attacks)
{
	string line;
	char nextChr;
	int x, y;

	HANDLE dir;
	WIN32_FIND_DATAA fileData;

	string atkSuffix = "*.attack";
	string dirBack = (dirPath.back() == '\\' || dirPath.back() == '/') ? "" : "\\";
	string path = dirPath + dirBack;

	// find attack file
	dir = FindFirstFileA((path + atkSuffix).c_str(), &fileData);
	if (dir == INVALID_HANDLE_VALUE) // check if file successfully opened
	{
		return -1;
	}

	ifstream atkFile(path + fileData.cFileName);
	if (!atkFile.is_open())
	{
		return -1;
	}

	while (getline(atkFile, line))
	{
		if (line == "")
		{
			continue;
		}
		if (line.back() == '\r')
		{
			line.back() = ' ';
		}
		x = -1;
		y = -1;
		stringstream lineStream(line);
		lineStream >> y;  //read y coor
		if (y < 1 || y > ROW_SIZE)
		{
			continue;
		}

		while (lineStream >> nextChr && nextChr == ' ') {} //seek comma

		if (lineStream.eof() || nextChr != ',')
		{
			continue;
		}

		lineStream >> x;                                    //read x coor
		if (x < 1 || x > COL_SIZE)
		{
			continue;
		}

		attacks.push(make_pair(y, x));
	}
	atkFile.close();
	return 1;
}
