#pragma once
#include "IBattleshipGameAlgo.h"
#include "CommonUtilities.h"
#include "Logger.h"


typedef IBattleshipGameAlgo *(*GetAlgoFuncType)();


class AlgorithmLoader
{
public:
	AlgorithmLoader() = delete;
	AlgorithmLoader(std::shared_ptr<Logger> pLogger) : _pLogger(pLogger){}
	~AlgorithmLoader();
	// blocking copy ctor & assignment operator (rule of three)
	AlgorithmLoader(const AlgorithmLoader& dllMngr) = delete;
	AlgorithmLoader& operator=(const AlgorithmLoader& dllMngr) = delete;

	void loadLibs(const std::vector<std::string>& dlls, std::vector<GetAlgoFuncType>& players, std::vector<std::string>& playerNames, const std::string& dirPath);
	IBattleshipGameAlgo* loadAlgo(int n);
	
	//size_t size() const { return libs.size(); }

private:
	//data members
	std::vector<std::tuple<std::string, HINSTANCE, GetAlgoFuncType>> libs;
	std::shared_ptr<Logger> _pLogger;

	/* removes the suffix (what comes after the dot) from filename - for getting player names from dll file names*/
	static std::string removeSuffix(const std::string& filename);
};
