#pragma once
#include "IBattleshipGameAlgo.h"
#include <vector>
#include <string>
#include <iostream>
#include <tuple>
#include <windows.h>

typedef IBattleshipGameAlgo *(*GetAlgoFuncType)();

class DLLManager
{
public:
	DLLManager();
	~DLLManager();
	IBattleshipGameAlgo* loadAlgo(const std::string path);

private:
	std::vector<std::tuple<std::string, HINSTANCE, GetAlgoFuncType>> libs;
	int loadDLL(const std::string path);
};
