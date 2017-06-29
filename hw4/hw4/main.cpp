#include <iostream>
#include <string>
#include "Matrix2d.h"
#include "Matrix3d.h"

using namespace std;

template<typename Groups>
void print(const Groups& all_groups) 
{
	for (const auto& groupType : all_groups) 
	{
		cout << groupType.first << ":" << endl;
		for (const auto& groupOfType : groupType.second) 
		{
			for (const auto& coord : groupOfType) 
			{
				cout << "{ ";
				for (auto i : coord) 
				{
					cout << i << ' ';
				}
				cout << "} ";
			}
			cout << endl;
		}
	}
}

int main() {
	Matrix2d<char> m = { { 'a', 'A', 'a' },{ 'B', 'a', 'B' },{ 'B', 'a', 'B' } };
	auto all_groups = m.groupValues([](auto i) {return islower(i) ? "L" : "U"; });
	print(all_groups);

	Matrix3d<int> m2 = { { { 1, 2, 3 },{ 1, 2 },{ 1, 2 } },{ { 1, 2 },{ 1, 2, 3, 4 } } };
	auto all_groups2 = m2.groupValues([](auto i) {return i % 3 ? "!x3" : "x3"; });
	print(all_groups2);
}
