#include "AlgorithmLoader.h"

AlgorithmLoader::~AlgorithmLoader()
{
	for (auto itr = libs.begin(); itr != libs.end(); ++itr)
	{
		FreeLibrary(std::get<1>(*itr));
	}
}




int AlgorithmLoader::loadDLL(const std::string path)
{
	HINSTANCE hLib = LoadLibraryA(path.c_str());
	if (!hLib)
	{
		//todo - LOG
		std::cout << "Error: cannot load dll: " << path << std::endl;
		return -1;
	}

	GetAlgoFuncType getAlgoFunc = reinterpret_cast<GetAlgoFuncType>(GetProcAddress(hLib, "GetAlgorithm"));
	if (!getAlgoFunc)
	{
		//todo - LOG
		std::cout << "Error: cannot load dll: " << path << std::endl;
		return -1;
	}

	libs.push_back(make_tuple(path, hLib, getAlgoFunc));

	return 1;
}

void AlgorithmLoader::loadLibs(const std::vector<std::string> dlls)
{
	for (std::string path : dlls)
	{
		loadDLL(path);
	}
}

void AlgorithmLoader::exportAlgos(std::vector<GetAlgoFuncType>& algos)
{
	for (int i = 0; i < libs.size(); i++)
	{
		algos.push_back(std::get<2>(libs[i]));
	}
}

size_t AlgorithmLoader::size() const
{
	return libs.size();
}

IBattleshipGameAlgo* AlgorithmLoader::loadAlgo(int n)
{
	if (n < 0 || n > libs.size()-1)
	{
		return nullptr;
	}
	return std::get<2>(libs[n])();
}

