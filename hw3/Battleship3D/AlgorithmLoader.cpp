#include "AlgorithmLoader.h"
#include "InitUtilities.h"

using namespace std;

AlgorithmLoader::~AlgorithmLoader()
{
	for (auto itr = libs.begin(); itr != libs.end(); ++itr)
	{
		FreeLibrary(get<1>(*itr));
	}
}


void AlgorithmLoader::loadLibs(const vector<string>& dlls, vector<GetAlgoFuncType>& players, vector<string>& playerNames, const string& dirPath)
{
	for (auto fileName : dlls)
	{
		
		auto dllFullPath = dirPath + "\\" + fileName;
		_pLogger->writeToLog("Trying to load dll: " + dllFullPath);
		HINSTANCE hLib = LoadLibraryA(dllFullPath.c_str());
		if (!hLib)
		{
			_pLogger->writeToLog("Cannot load dll: " + dllFullPath + " (LoadLibraryA failed)", false, Logger::eLogType::LOG_WARNING);
			continue;
		}

		GetAlgoFuncType getAlgoFunc = reinterpret_cast<GetAlgoFuncType>(GetProcAddress(hLib, "GetAlgorithm"));
		if (!getAlgoFunc)
		{
			LPTSTR errorText = NULL;
			FormatMessage(
				// use system message tables to retrieve error text
				FORMAT_MESSAGE_FROM_SYSTEM
				// allocate buffer on local heap for error text
				| FORMAT_MESSAGE_ALLOCATE_BUFFER
				// Important! will fail otherwise, since we're not 
				// (and CANNOT) pass insertion parameters
				| FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,    // unused with FORMAT_MESSAGE_FROM_SYSTEM
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
				(LPTSTR)&errorText,  // output 
				0, // minimum size for output buffer
				NULL);
			if (NULL != errorText)
			{
				// ... do something with the string `errorText` - log it, display it to the user, etc.

				// release memory allocated by FormatMessage()
				wstring ws(errorText);
				_pLogger->writeToLog(string(ws.begin(), ws.end()) , false, Logger::eLogType::LOG_ERROR);
				LocalFree(errorText);
				errorText = NULL;
			}

			_pLogger->writeToLog("Cannot load dll: " + dllFullPath + " (GetProcAddress failed)", false, Logger::eLogType::LOG_WARNING);
			continue;
		}

		libs.push_back(make_tuple(dllFullPath, hLib, getAlgoFunc));
		// success! keep algo function and player name:
		_pLogger->writeToLog("Succeeded loading dll: " + dllFullPath);
		players.push_back(getAlgoFunc);
		playerNames.push_back(removeSuffix(fileName));
	}
}


string AlgorithmLoader::removeSuffix(const string& filename)
{
	auto lastdot = filename.find_last_of(".");
	if (lastdot == string::npos)
	{
		return filename;
	}
	return filename.substr(0, lastdot);
}

IBattleshipGameAlgo* AlgorithmLoader::loadAlgo(int n)
{
	if (n < 0 || n > libs.size()-1)
	{
		return nullptr;
	}
	return get<2>(libs[n])();
}

