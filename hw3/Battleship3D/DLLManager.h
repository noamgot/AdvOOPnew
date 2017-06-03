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

	void loadLibs(const std::vector<std::string> dlls);
	IBattleshipGameAlgo* loadAlgo(int n);
	int size() const;

private:
	int loadDLL(const std::string path);
};
