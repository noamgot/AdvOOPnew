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
	IBattleshipGameAlgo* loadAlgo(const std::string path);

private:
	int loadDLL(const std::string path);
};
