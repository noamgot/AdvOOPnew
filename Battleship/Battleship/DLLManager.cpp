#include "DLLManager.h"


DLLManager::DLLManager() {}

DLLManager::~DLLManager()
{
	for (auto itr = libs.begin(); itr != libs.end(); ++itr)
	{
		FreeLibrary(std::get<1>(*itr));
	}
}

int DLLManager::loadDLL(const std::string path)
{
	HINSTANCE hLib = LoadLibraryA(path.c_str());
	if (!hLib)
	{
		std::cout << "Cannot load dll: " << path << std::endl;
		return -1;
	}

	GetAlgoFuncType getAlgoFunc = (GetAlgoFuncType)GetProcAddress(hLib, "GetAlgorithm");
	if (!getAlgoFunc)
	{
		std::cout << "Cannot load function GetAlgorithm()" << std::endl;
		return -1;
	}

	libs.push_back(make_tuple(path, hLib, getAlgoFunc));

	return 1;
}

IBattleshipGameAlgo* DLLManager::loadAlgo(const std::string path)
{
	if (loadDLL(path) == -1)
	{
		return nullptr;
	}
	return std::get<2>(libs.back())();
}

