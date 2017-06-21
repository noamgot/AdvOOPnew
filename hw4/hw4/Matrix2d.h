#pragma once
#include "Coord2d.h"
#include <initializer_list>
#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include <functional>
#include <vector>

template<typename T>
class Matrix2d 
{
public:
	int rows;
	int cols;
	std::vector<std::vector<T>> arr;
	
	Matrix2d(std::initializer_list<std::initializer_list<T>> v) :
	rows(v.size()), cols(v.end()-v.begin()), arr(rows, std::vector<T>(cols))
	{
		int i = 0;
		for (auto& row : v) 
		{
			int j = 0;
			for (auto& a : row) {
				arr[i][j++] = a;
			}
			i++;
		}
	}

	template<class GroupingFunc>
	auto groupValues(GroupingFunc groupingFunc)
	{
		using GroupingType = std::result_of_t<GroupingFunc(T&)>;
		std::map<GroupingType, std::list<std::list<Coord2d>>> groups;
		std::vector<std::vector<GroupingType>> groupMap(rows, std::vector<GroupingType>(cols));
		std::vector<std::vector<bool>> visitMap(rows, std::vector<bool>(cols));
		//GroupingType groupMap[rows][cols];
		//bool visitMap[rows][cols];

		for (auto i = 0; i < rows; i++)
		{
			for (auto j = 0; j < cols; j++)
			{
				groupMap[i][j] = groupingFunc(arr[i][j]);
				visitMap[i][j] = false;
			}
		}
		for (auto i = 0; i < rows; i++)
		{
			for (auto j = 0; j < cols; j++)
			{
				if (!visitMap[i][j])
				{
					// start bfs
					GroupingType groupParent = groupMap[i][j];
					std::list<Coord2d> queue;	// bfs queue
					std::list<Coord2d> group;	// list of the bfs tree nodes

					visitMap[i][j] = true;
					queue.push_back(Coord2d(i, j));
					group.push_back(Coord2d(i, j));

					while (!queue.empty())
					{
						Coord2d c = queue.front();
						queue.pop_front();

						// check all adjacent cells if visited and are the same type
						if (i != 0 && !visitMap[i - 1][j] && groupMap[i - 1][j] == groupParent)
						{
							queue.push_back(Coord2d(i - 1, j));
							group.push_back(Coord2d(i - 1, j));
							visitMap[i - 1][j] = true;
						}
						if (i != rows - 1 && !visitMap[i + 1][j] && groupMap[i + 1][j] == groupParent)
						{
							queue.push_back(Coord2d(i + 1, j));
							group.push_back(Coord2d(i + 1, j));
							visitMap[i + 1][j] = true;
						}
						if (j != 0 && !visitMap[i][j - 1] && groupMap[i][j - 1] == groupParent)
						{
							queue.push_back(Coord2d(i, j - 1));
							group.push_back(Coord2d(i, j - 1));
							visitMap[i][j - 1] = true;
						}
						if (j != cols - 1 && !visitMap[i][j + 1] && groupMap[i][j + 1] == groupParent)
						{
							queue.push_back(Coord2d(i, j + 1));
							group.push_back(Coord2d(i, j + 1));
							visitMap[i][j + 1] = true;
						}
					}
					groups[groupParent].push_back(group);
				}
			}
		}
		return groups;
	}

};
