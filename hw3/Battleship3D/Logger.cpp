#include "Logger.h"
#include <iostream>
#include <ctime>
#include <mutex>

#define MAX_TIME_BUF_SIZE 64

Logger::Logger(const std::string& dirPath, const std::string& fileName) 
				: _numWarnings(0U), _numErrors(0U)
{
	_logFile.open(dirPath + "\\" + fileName);
	// Write the first lines
	if (_logFile.is_open())
	{
		_logFile << getCurrentTime().c_str();
		_logFile << " [START] LOG FILE CREATED. BATTLESHIP COMPETITION HAS STARTED RUNNING." << std::endl;
		_logFile << "#################################################################################" << std::endl;
	}

}


Logger::~Logger()
{
	if (_logFile.is_open())
	{
		_logFile << getCurrentTime().c_str();
		_logFile << " [END]: GAME HAS ENDED. EXITING...\n" << std::endl;

		// Report number of errors and warnings
		_logFile << _numWarnings << " warnings" << std::endl;
		_logFile << _numErrors << " errors" << std::endl;
		_logFile.close();
	}
}

void Logger::writeToLog(const std::string msg, bool writeToConsole, const eLogType logType)
{
	std::lock_guard<std::mutex> mlock(_mutex);
	*this << logType << msg;
	if (writeToConsole)
	{
		std::cout << msg.c_str() << std::endl;
	}
}
std::string Logger::getCurrentTime()
{
	char timeBuf[MAX_TIME_BUF_SIZE];
	auto currentTime = std::time(nullptr);
	ctime_s(timeBuf, sizeof timeBuf, &currentTime);
	std::string timeStr(timeBuf);
	timeStr.pop_back(); // remove \n
	return timeStr;
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
