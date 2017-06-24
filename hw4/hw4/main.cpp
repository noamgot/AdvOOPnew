#include <locale>
#include <ctype.h>
#include <iostream>
#include "Matrix2d.h"
#include "Matrix3d.h"

//#include "Matrix.h"

using namespace std;

template<typename Groups>
void print(const Groups& all_groups) 
{
	for (const auto& groupType : all_groups) 
	{
		std::cout << groupType.first << ":" << std::endl;
		for (const auto& groupOfType : groupType.second) 
		{
			for (const auto& coord : groupOfType) 
			{
				std::cout << "{ ";
				for (auto i : coord) 
				{
					std::cout << i << ' ';
				}
				std::cout << "} ";
			}
			std::cout << std::endl;
		}
	}
}

int main() {
	Matrix2d<char> m = { { 'a', 'A', 'a' },{ 'B', 'a', 'B' },{ 'B', 'a', 'B' } };
	//auto all_groups = m.groupValues([](auto i) {return std::islower(i) ? "L" : "U"; });
	auto all_groups = m.groupValues([](auto i) {return islower(i) ? "l" : "u"; });
	print(all_groups);
	Matrix3d<int> m2 = { { { 1, 2, 3 },{ 1, 2 },{ 1, 2 } },{ { 1, 2 },{ 1, 2, 3, 4 } } };
	auto groups = m2.groupValues([](auto i) {return i % 3 ? "!x3" : "x3"; });
	print(groups);


	
}
