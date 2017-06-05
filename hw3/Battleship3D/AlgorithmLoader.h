#pragma once
#include "IBattleshipGameAlgo.h"
#include "CommonUtilities.h"


typedef IBattleshipGameAlgo *(*GetAlgoFuncType)();

class AlgorithmLoader
{
	//data members
	std::vector<std::tuple<std::string, HINSTANCE, GetAlgoFuncType>> libs;

public:
	AlgorithmLoader(){}
	~AlgorithmLoader();
	// blocking copy ctor & assignment operator (rule of three)
	AlgorithmLoader(const AlgorithmLoader& dllMngr) = delete;
	AlgorithmLoader& operator=(const AlgorithmLoader& dllMngr) = delete;

	void loadLibs(const std::vector<std::string> dlls);
	void exportAlgos(std::vector<GetAlgoFuncType>& algos);
	IBattleshipGameAlgo* loadAlgo(int n);
	int size() const;

private:
	int loadDLL(const std::string path);
};
