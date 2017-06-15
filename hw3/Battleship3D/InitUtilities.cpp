#include "InitUtilities.h"
#include <direct.h>
#include <iostream>
#include <string>
#include <Windows.h>
#include <set>
#include <bitset>
#include <bitset>
#include <bitset>
#include <bitset>
#include <algorithm>

using namespace std;
using namespace CommonUtilities;


namespace InitUtilities
{

	/* input argument for setting the amount of threads */
	const string PARAM_THREADS("-threads");
	const string BOARD_FILE_SUFFIX(".sboard");
	const string LIB_FILE_SUFFIX(".dll");
	const string CONFIG_FILE_SUFFIX(".config");

	int processInputArguments(int argc, char** argv, string& dirPath, int& numThreads)
	{
		char *p;
		auto gotDirPath = false;
		if (argc >= 2)
		{
			// we accept the arguments in any order, 
			// and we assume that if a folder path is given it is the first argument
			for (auto i = 1; i < argc; ++i)
			{
				if (string(argv[i]) == PARAM_THREADS)
				{
					if (i + 1 < argc)
					{
						int _numThreads = strtol(argv[i + 1], &p, 10);
						if (!*p && _numThreads > 0)
						{
							numThreads = _numThreads;
							break;
						}
					}
					// if there's not a valid positive after PARAM_THREADS - ignore...
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
		// convert the given directory path to full path
		if (convertToFullPath(dirPath) < 0)
		{
			return -1;
		}
		if (!isValidPath(dirPath))
		{
			cout << "Wrong Path: " << dirPath << endl;
			return -1;
		}
		return 0;
	}

	string getDirPath()
	{
		char buff[BUF_SIZE];
		if (!_getcwd(buff, BUF_SIZE - 1))
		{
			return string(); //signs the string is bad
		}
		string temp(buff);
		return temp;
	}

	int convertToFullPath(string& dirPath)
	{
		if (dirPath.empty()) //error occurred in getDirPath()
		{
			return -1;
		}
		// get the full path of the directory
		char fullPath[BUF_SIZE];
		if (!_fullpath(fullPath, dirPath.c_str(), BUF_SIZE - 1))
		{
			cout << "Error: conversion of directory path to full path failed" << endl;
			return -1;
		}
		// copy the full path to dirPath
		dirPath = fullPath;
		// clear "\\" if added by _fullpath()
		if (dirPath.back() == '\\')
		{
			dirPath.pop_back();
		}
		return 0;
	}

	bool isValidPath(const string dirPath)
	{
		auto dwAttrib = GetFileAttributesA(dirPath.c_str());
		return dwAttrib != INVALID_FILE_ATTRIBUTES &&
			dwAttrib & FILE_ATTRIBUTE_DIRECTORY;
	}

	int getDirectoryFileList(const string dirPath, vector<string>& fileListVector, shared_ptr<Logger> pLogger)
	{
		pLogger->writeToLog("Getting directory file list");
		fileListVector.clear();
		auto sysDIR("2>NUL dir /a-d /b \"" + dirPath + "\"");
		char buffer[BUF_SIZE];
		// parse directory and find the wanted file
		auto fileList = _popen(sysDIR.c_str(), "r");
		if (!fileList)
		{
			char errMsg[BUF_SIZE];
			if (strerror_s(errMsg, BUF_SIZE, errno))
			{
				pLogger->writeToLog("Error: strerror_s failed", true, Logger::eLogType::LOG_ERROR);
			}
			else
			{
				pLogger->writeToLog("Error: " + string(errMsg), true, Logger::eLogType::LOG_ERROR);
			}
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

	void filterDirFiles(const vector<string>& dirFiles, vector<string>& boardFiles, vector<string>& dllFiles, string& configFile)
	{
		// copy all relevant files to the filteredFileList vector
		for (auto& file : dirFiles)
		{
			if (endsWith(file, BOARD_FILE_SUFFIX))
			{
				boardFiles.push_back(file); 
			}
			else if (endsWith(file, LIB_FILE_SUFFIX))
			{
				dllFiles.push_back(file);
			} 
			else if (endsWith(file, CONFIG_FILE_SUFFIX))
			{
				configFile = file;
			}
		}
	}

	bool endsWith(const string line, const string suffix)
	{
		auto pos = line.rfind(suffix);
		return pos != string::npos && pos == line.length() - suffix.length();
	}

	int loadConfig(const string dirPath, const string cfgPath, int& numThreads, shared_ptr<Logger> pLogger)
	{
		pLogger->writeToLog("Processing configuration file...");
		char* p;
		string line;
		size_t pos;
		vector<string> args;
		ifstream cfgFile(dirPath + "\\" + cfgPath);
		if (!cfgFile.is_open())
		{
			pLogger->writeToLog("Opening configuration file failed", false, Logger::eLogType::LOG_WARNING);
			return -1;
		}

		getline(cfgFile, line);
		if (cfgFile.fail())
		{
			pLogger->writeToLog("Failed to read from config file, or config file is empty", false, Logger::eLogType::LOG_WARNING);
			return -1;
		}

		while ((pos = line.find(' ')) != string::npos) {
			string arg = line.substr(0, pos);
			args.push_back(arg);
			line.erase(0, pos + 1);
		}
		args.push_back(line);

		for (auto i = 0; i < args.size(); ++i)
		{
			if (args[i] == PARAM_THREADS)
			{
				if (i + 1 < args.size())
				{
					int _numThreads = strtol(args[i + 1].c_str(), &p, 10);
					if (!*p && _numThreads > 0)
					{
						numThreads = _numThreads;
						return 1;
					}
				}
			}
		}
		return 1;
	}

	int checkMinBoardsAndPlayersCount(size_t boardsCnt, size_t playersCnt, const string& dirPath,
		bool filteredLists, shared_ptr<Logger> pLogger)
	{
		auto error = false;
		string msg;
		if (boardsCnt == 0)
		{
			if (filteredLists)
			{
				msg = "No valid board files available";
			}
			else
			{
				msg = "No board files (*.sboard) looking in path: " + dirPath;
			}
			pLogger->writeToLog(msg, true, Logger::eLogType::LOG_ERROR);
			error = true;
		}
		if (playersCnt < 2)
		{
			if (filteredLists)
			{
				msg = "Not enough valid algorithms (loaded " + to_string(playersCnt) + ", needs at least 2)";
			}
			else
			{
				msg = "Missing algorithm (dll) files looking in path: " + dirPath + " (needs at least two)";
			}
			pLogger->writeToLog(msg, true, Logger::eLogType::LOG_ERROR);
			error = true;
		}
		if (error)
		{
			return -1;
		}
		return 0;
	}

	void initBoards3D(const vector<string>& boardPaths, vector<MyBoardData>& boards, const std::string& dirPath, shared_ptr<Logger> pLogger)
	{
		pLogger->writeToLog("Processing boards...");
		for (auto& boardPath : boardPaths)
		{
			auto fullPath = dirPath + "\\" + boardPath;
			pLogger->writeToLog("Starting to process board in path: " + fullPath);
			auto rows = 0, cols = 0, depth = 0;
			string line;
			ifstream boardFile(fullPath);
			if (!boardFile.is_open())
			{
				pLogger->writeToLog("Opening board file failed", false, Logger::eLogType::LOG_WARNING);
				continue;
			}
			set<char> charSet{ WATER, BOAT,MISSLE_SHIP, SUBMARINE, DESTROYER, char(tolower(BOAT)),
				char(tolower(MISSLE_SHIP)), char(tolower(SUBMARINE)), char(tolower(DESTROYER)) };

			getline(boardFile, line);
			if (boardFile.eof() || boardFile.fail())
			{
				pLogger->writeToLog("Failed to read from file, or reached EOF", false, Logger::eLogType::LOG_WARNING);
				continue;
			}
			if (getDims(line, rows, cols, depth) <= 0)
			{
				pLogger->writeToLog("Invalid dimensions line", false, Logger::eLogType::LOG_WARNING);
				continue;
			}

			MyBoardData board(rows, cols, depth);

			// if first line after dimensions is not empty - bad format
			getline(boardFile, line);
			if (!boardFile.eof() && !line.empty())
			{
				pLogger->writeToLog("Non-empty line after dimensions line - bad board format", false, Logger::eLogType::LOG_WARNING);
				continue;
			}
			getline(boardFile, line);

			for (auto k = 0; k < depth; k++)
			{
				for (auto i = 0; i < rows; i++)
				{
					for (auto j = 0; j < cols; j++)
					{
						if (boardFile.eof() || line.empty() || j >= line.size())
						{
							board[i][j][k] = WATER;
						}
						else
						{
							board[i][j][k] = charSet.find(line[j]) == charSet.end() ? WATER : line[j];
						}
					}
					if (!boardFile.eof() && !line.empty() && i != rows - 1)
					{
						getline(boardFile, line);
					}
				}
				if (boardFile.eof())
				{
					continue;
				}
				while (getline(boardFile, line))
				{
					if (line.empty())
					{
						getline(boardFile, line);
						break;
					}
				}
			}
			boardFile.close();
			if (checkBoard3D(board, pLogger) < 0)
			{
				continue;
			}
			pLogger->writeToLog("Success!");
			boards.push_back(board);
		}
	}

	int getDims(const string& line, int& rows, int& cols, int& depth)
	{
		auto j = 0;
		// skip preceding white space
		while (line[j] == ' ' || line[j] == '\t')
		{
			j++;
		}
		for (auto i = 0; i <= 2; i++)
		{
			string buff = "";
			while (j < line.size() && tolower(line[j]) != 'x')
			{
				if (isdigit(line[j]) == 0)
				{
					// ignore following white space
					while (i == 2 && j < line.size() && (line[j] == ' ' || line[j] == '\t'))
					{
						j++;
					}
					if (j == line.size())
					{
						break;
					}
					return -1;
				}
				buff += line[j];
				j++;
			}
			j++;
			if (i == 0)
			{
				cols = stoi(buff);
			}
			if (i == 1)
			{
				rows = stoi(buff);
			}
			if (i == 2)
			{
				depth = stoi(buff);
			}
		}
		return rows*cols*depth;
	}

	int checkBoard3D(MyBoardData& board, shared_ptr<Logger> pLogger)
	{
		auto rows = board.rows();
		auto cols = board.cols();
		auto depth = board.depth();
		auto shipCountA = 0, shipCountB = 0, isShipA = 0, isShipB = 0, adjCheck = 0;
		map<char, int> shipsA = { { BOAT,1 },{ MISSLE_SHIP,2 },{ SUBMARINE,3 },{ DESTROYER,4 } };
		map<char, int> shipsB = { { char(tolower(BOAT)),1 },{ char(tolower(MISSLE_SHIP)),2 },
		{ char(tolower(SUBMARINE)),3 },{ char(tolower(DESTROYER)),4 } };
		bitset<4> errShipsA, errShipsB;// error flags for ship misshapes

		for (auto k = 0; k < depth; k++)
		{
			for (auto i = 0; i < rows; i++)
			{
				for (auto j = 0; j < cols; j++)
				{
					if (board[i][j][k] != WATER)
					{
						isShipA = shipsA[board[i][j][k]] != 0;       // 1 if its a ship, otherwise 0
						isShipB = shipsB[board[i][j][k]] != 0;
						if (isShipA || isShipB)
						{
							// check if its new
							if (!(k != 0 && board[i][j][k - 1] == board[i][j][k] ||
								i != 0 && board[i - 1][j][k] == board[i][j][k] ||
								j != 0 && board[i][j - 1][k] == board[i][j][k]))
							{
								// check for misshape
								if (checkShape3D(board, shipsB[tolower(board[i][j][k])], i, j, k) < 0)
								{
									if (isShipA)
									{
										errShipsA[shipsA[board[i][j][k]] - 1] = true;
									}
									if (isShipB)
									{
										errShipsB[shipsB[board[i][j][k]] - 1] = true;
									}
									//return -1;
								}
								else
								{
									shipCountA += isShipA;
									shipCountB += isShipB;
								}
							}
							// Check if any adjacent ships exist
							if (k != 0 && board[i][j][k - 1] != board[i][j][k] && board[i][j][k - 1] != ' ' ||
								i != 0 && board[i - 1][j][k] != board[i][j][k] && board[i - 1][j][k] != ' ' ||
								j != 0 && board[i][j - 1][k] != board[i][j][k] && board[i][j - 1][k] != ' ')
							{
								adjCheck = 1;
							}
						}
					}
				}
			}
		}
		return logBoardErrors(errShipsA, errShipsB, shipCountA, shipCountB, adjCheck, board, pLogger);
	}

	int checkShape3D(MyBoardData& board, const int size, int i, int j, int k)
	{
		//todo @ben - code duplication
		auto rows = board.rows();
		auto cols = board.cols();
		auto depth = board.depth();
		auto verL = 1, horL = 1, depL = 1, maxL = 0;
		// run horizontally, check above, below, inwards and outwards
		while (j + horL < cols && board[i][j][k] == board[i][j + horL][k])
		{
			if ((i + 1 < rows && board[i][j][k] == board[i + 1][j + horL][k]) ||
				(i - 1 >= 0 && board[i][j][k] == board[i - 1][j + horL][k]) ||
				(k + 1 < depth && board[i][j][k] == board[i][j + horL][k + 1]) ||
				(k - 1 >= 0 && board[i][j][k] == board[i][j + horL][k - 1]))
			{
				return -1;
			}
			horL++;
		}
		// run vertically, check right, left, inwards and outwards
		while (i + verL < rows && board[i][j][k] == board[i + verL][j][k])
		{
			if ((j + 1 < cols && board[i][j][k] == board[i + verL][j + 1][k]) ||
				(j - 1 >= 0 && board[i][j][k] == board[i + verL][j - 1][k]) ||
				(k + 1 < depth && board[i][j][k] == board[i + verL][j][k + 1]) ||
				(k - 1 >= 0 && board[i][j][k] == board[i + verL][j][k - 1]))
			{
				return -1;
			}
			verL++;
		}
		// run depth, check right, left, above and below
		while (k + depL < depth && board[i][j][k] == board[i][j][k + depL])
		{
			if ((j + 1 < cols && board[i][j][k] == board[i][j + 1][k + depL]) ||
				(j - 1 >= 0 && board[i][j][k] == board[i][j - 1][k + depL]) ||
				(i + 1 < rows && board[i][j][k] == board[i + 1][j][k + depL]) ||
				(i - 1 >= 0 && board[i][j][k] == board[i - 1][j][k + depL]))
			{
				return -1;
			}
			depL++;
		}

		// check for misshape in size (no dimension matches ship size or 2 or more dimensions are longer than 1 cell)
		if ((horL != size && verL != size && depL != size) || (horL > 1 && verL > 1) || (horL > 1 && depL > 1) || (verL > 1 && depL > 1))
		{
			return -1;
		}

		// call for a ship register, the direction which it's length equals the size of the ship is the dierction of the ship
		registerShip(board, size, i, j, k, verL == size ? 1 : 0, horL == size ? 1 : 0, depL == size ? 1 : 0);

		return 1;
	}

	void registerShip(MyBoardData& board, const int size, int i, int j, int k, int iDir, int jDir, int kDir)
	{
		auto type = charToShipType(board[i][j][k]);
		int playernum = isupper(board[i][j][k]) ? 0 : 1;
		map<Coordinate, bool> shipMap;
		for (int l = 0; l < size; l++)
		{
			shipMap[Coordinate(i+1,j+1,k+1)] = true;
			k += kDir;
			i += iDir;
			j += jDir;
		}
		board.addShip(playernum, Ship(size, type, shipMap));
	}

	void initIndividualBoards(const vector<MyBoardData>& boards, vector<MyBoardData>& boardsA, vector<MyBoardData>& boardsB)
	{
		// we know that boardsA.size() == boardsB.size() == boards.size() - no need to check
		for (auto b = 0; b < boards.size(); b++)
		{
			// set dimensions for the individual boards
			boardsA[b] = MyBoardData(boards[b].rows(), boards[b].cols(), boards[b].depth());
			boardsB[b] = MyBoardData(boards[b].rows(), boards[b].cols(), boards[b].depth());
			for (auto i = 1; i <= boards[b].rows(); ++i)
			{
				for (auto j = 1; j <= boards[b].cols(); ++j)
				{
					for (auto k = 1; k <= boards[b].depth(); k++)
					{
						Coordinate coor(i, j, k);
						auto c = boards[b].charAt(coor);
						if (isalpha(c)) //part of a ship
						{
							if (isupper(c)) // a ship of A
							{
								boardsA[b].setChar(coor, c);
								boardsB[b].setChar(coor, WATER);
							}
							else // a ship of B
							{
								boardsA[b].setChar(coor, WATER);
								boardsB[b].setChar(coor, c);
							}
						}
						else // a space - update both boards
						{
							boardsA[b].setChar(coor, WATER);
							boardsB[b].setChar(coor, WATER);
						}
					}
				}
			}
		}
		
	}

	int logBoardErrors(bitset<4>& errShipsA, bitset<4>& errShipsB, int shipCountA, int shipCountB, int adjCheck, const MyBoardData& board, shared_ptr<Logger> pLogger)
	{
		auto ret = 0;
		// Print possible errors
		logWrongSizeOrShapeError(errShipsA, ret, "A", pLogger);
		logWrongSizeOrShapeError(errShipsB, ret, "B", pLogger);
		logBadShipsCountError(shipCountA, shipCountB, board, pLogger);
		if (adjCheck)
		{
			pLogger->writeToLog("Adjacent Ships on Board", false, Logger::eLogType::LOG_WARNING);
			ret = -1;
		}
		return ret;
	}

	void logWrongSizeOrShapeError(bitset<4>& errShips, int& ret, const string player, shared_ptr<Logger> pLogger)
	{
		char shipMap[4] = { BOAT,MISSLE_SHIP,SUBMARINE,DESTROYER };
		if (player == "B")
		{
			for (auto& c : shipMap)
			{
				c = tolower(c);
			}
		}
		for (auto i = 0; i < 4; i++)
		{
			if (errShips[i])
			{
				pLogger->writeToLog((static_cast<string>("Wrong size or shape for ship ") += shipMap[i]) + " for player " + player,
									false, Logger::eLogType::LOG_WARNING);
				ret = -1;
			}
		}
	}

	void logBadShipsCountError(int shipCountA, int shipCountB, const MyBoardData& board, shared_ptr<Logger> pLogger)
	{
		if (shipCountA != shipCountB)
		{
			pLogger->writeToLog("Ships number is not equal for both players: A has " + to_string(shipCountA) + 
								", B has " + to_string(shipCountB), false, Logger::eLogType::LOG_WARNING);
		}
		else
		{
			vector<Ship> shipsA = board.getShipList(0);
			vector<Ship> shipsB = board.getShipList(1);
			sort(shipsA.begin(), shipsA.end());
			sort(shipsB.begin(), shipsB.end());
			for (auto i = 0; i < shipCountA; i++)
			{
				if (shipsA[i].getType() != shipsB[i].getType())
				{
					pLogger->writeToLog("Players have different ship types", false, Logger::eLogType::LOG_WARNING);
					return;
				}
			}
		}
	}

	Ship handleShipDiscovery(int iOrig, int jOrig, int numOfRows, int numOfCols, const char** board)
	{
		auto i = iOrig;
		auto j = jOrig;
		auto size = 0;
		map<pair<int, int>, bool> coordinates;
		auto c = board[i][j];
		// we will iterate only downwards or rightwards
		do
		{
			// remember that we save the coordinates from in the form of 1 to ROW/COL SIZE (and not starting from 0)
			// hence we give a +1 offset
			coordinates[make_pair(i + 1, j + 1)] = true;
			size++;
		} while (++i < numOfRows && board[i][j] == c); // checking downwards
		i = iOrig;
		while (++j < numOfCols && board[i][j] == c) // checking rightwards
		{
			coordinates[make_pair(i + 1, j + 1)] = true;
			size++;
		}
		//return Ship(size, charToShipType(c), coordinates);
		return Ship(); // todo - change!!!!!! just for tests...
		
	}
}
