#include "Logger.h"
#include <iostream>
#include <ctime>
#include <mutex>
#include <sys/types.h>
#include <sys/stat.h>

using namespace std;

#define MAX_TIME_BUF_SIZE 64

Logger::Logger(const string& dirPath, const string& fileName, long maxLogFileSize)
				: _numWarnings(0U), _numErrors(0U)
{
	
	auto fullPath = dirPath + "\\" + fileName;
	auto fileSize = getFileSize(fullPath);
	if (fileSize < 0 || fileSize > maxLogFileSize) // file does not exist or is already too big
	{
		_logFile.open(fullPath); // open log file regularly
	}
	else
	{
		_logFile.open(fullPath, ios_base::app);
	}
	// Write the first lines
	if (_logFile.is_open())
	{
		_logFile << "##############################################################################################" << endl;
		_logFile << getCurrentTime().c_str();
		_logFile << " [START] LOG FILE CREATED. BATTLESHIP COMPETITION HAS STARTED RUNNING." << endl;
		_logFile << "##############################################################################################" << endl;
		
	}

}


Logger::~Logger()
{
	if (_logFile.is_open())
	{
		_logFile << getCurrentTime().c_str();
		_logFile << " [END]: GAME HAS ENDED. EXITING...\n" << endl;

		// Report number of errors and warnings
		_logFile << _numWarnings << " warnings" << endl;
		_logFile << _numErrors << " errors" << endl << endl;
		_logFile.close();
	}
}

void Logger::writeToLog(const string msg, bool writeToConsole, const eLogType logType)
{
	lock_guard<mutex> mlock(_mutex);
	*this << logType << msg;
	if (writeToConsole)
	{
		cout << msg.c_str() << endl;
	}
}
string Logger::getCurrentTime()
{
	char timeBuf[MAX_TIME_BUF_SIZE];
	auto currentTime = time(nullptr);
	ctime_s(timeBuf, sizeof timeBuf, &currentTime);
	string timeStr(timeBuf);
	timeStr.pop_back(); // remove \n
	return timeStr;
}

long Logger::getFileSize(string filename)
{
	struct _stat statBuf;
	auto rc = _stat(filename.c_str(), &statBuf);
	return rc == 0 ? statBuf.st_size : -1;	
}

Logger& operator<<(Logger& logger, const Logger::eLogType logType)
{
	logger._logFile << Logger::getCurrentTime().c_str() << " ";

	switch (logType) 
	{
		case Logger::eLogType::LOG_ERROR:
			logger._logFile << "[!!!ERROR!!!]: ";
			logger._numErrors++;
			break;

		case Logger::eLogType::LOG_WARNING:
			logger._logFile << "[!WARNING!]: ";
			logger._numWarnings++;
			break;
		default:
			logger._logFile << "[INFO]: ";
			break;
	}
	return logger;
}
