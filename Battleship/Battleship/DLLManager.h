#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include <tuple>
#include <windows.h>

typedef IBattleshipGameAlgo *(*GetAlgoFuncType)();

class DLLManager
{
	//data members
	std::vector<std::tuple<std::string, HINSTANCE, GetAlgoFuncType>> libs;

public:
	DLLManager(){}
	~DLLManager();
	// blocking copy ctor & assignment operator (rule of three)
	DLLManager(const DLLManager& dllMngr) = delete;
	DLLManager& operator=(const DLLManager& dllMngr) = delete;

	IBattleshipGameAlgo* loadAlgo(const std::string path);

private:
	int loadDLL(const std::string path);
};
