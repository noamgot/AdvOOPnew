#pragma once
#include "Coordinate.h"
#include <initializer_list>
#include <algorithm>
#include <list>
#include <map>
#include <functional>
#include <vector>

template<typename T>
class Matrix2d 
{
	std::vector<T> _array;
	std::vector<int> _dimensions;
	int _size;
public:
	Matrix2d(std::initializer_list<std::initializer_list<T>> v);

	template<class GroupingFunc>
	auto groupValues(GroupingFunc groupingFunc);
};

template<typename T>
Matrix2d<T>::Matrix2d(std::initializer_list<std::initializer_list<T>> v) : _dimensions(2)
{
	// First, let us find the REAL dimensions of the
	auto row_counter = 0;
	auto col_counter = 0;
	for (auto& row : v)
	{
		row_counter++;
		col_counter = 0;
		for (auto& a : row)
		{
			col_counter++;
		}
		if (col_counter > _dimensions[1]) { _dimensions[1] = col_counter; }
	}
	if (row_counter > _dimensions[0]) { _dimensions[0] = row_counter; }


	_size = _dimensions[1] * _dimensions[0];
	_array = std::vector<T>(_size);

	row_counter = 0;
	for (auto& row : v)
	{
		col_counter = 0;
		for (auto& col : row)
		{
			_array[row_counter + col_counter] = col;
			col_counter++;
		}
		row_counter += _dimensions[1];
	}
}

template<typename T>
template<class GroupingFunc>
auto Matrix2d<T>::groupValues(GroupingFunc groupingFunc)
{
	using GroupingType = std::result_of_t<GroupingFunc(T&)>;
	std::map<GroupingType, std::vector<std::vector<Coordinate>>> groups;
	std::vector<GroupingType> groupMapT(_size);
	std::vector<bool> visitMapT(_size);
	for (int i = 0; i < _size; i++)
	{
		groupMapT[i] = groupingFunc(_array[i]);
	}

	// run through the whole matrix
	for (int i = 0; i < _size; i++)
	{
		// start BFS if found unvisited node
		// Adjacent Nodes: Coord1 and Coord2 are neighbors if they have the same GroupingType and |Coord1-Coord2| = 1
		if (!visitMapT[i])
		{
			GroupingType groupParent = groupMapT[i];
			std::list<Coordinate> queue;	// bfs queue
			std::vector<Coordinate> group;	// list of the bfs tree nodes

			Coordinate curr(i, _dimensions); // get coord
			visitMapT[i] = true;
			queue.push_back(curr);
			group.push_back(curr);

			while (!queue.empty())
			{
				Coordinate c = queue.front();
				queue.pop_front();
				auto arrayLoc = c.getMatloc();
				auto dimOffset = 1;
				for (int j = static_cast<int>(_dimensions.size()) - 1; j >= 0; j--)
				{
					auto back = arrayLoc - dimOffset;
					auto front = arrayLoc + dimOffset;
					// check back and front in dimension j for unvisited neighbor nodes
					if (c[j] != 0 && !visitMapT[back] && groupMapT[back] == groupParent)
					{
						queue.push_back(c.changeDim(j, -1, back));
						group.push_back(c.changeDim(j, -1, back));
						visitMapT[back] = true;
					}
					if (c[j] != _dimensions[j] - 1 && !visitMapT[front] && groupMapT[front] == groupParent)
					{
						queue.push_back(c.changeDim(j, 1, front));
						group.push_back(c.changeDim(j, 1, front));
						visitMapT[front] = true;
					}
					dimOffset *= _dimensions[j];
				}
			}
			std::sort(group.begin(), group.end());
			groups[groupParent].push_back(group);
		}
	}
	return groups;
}


