#pragma once
#include "Coordinate.h"
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
		//ben test
		std::vector<T> _array;
		std::vector<int> _dimensions;
		size_t _size;
		
		int _rows;
		int _cols;
		Matrix2d(std::initializer_list<std::initializer_list<T>> v) : _rows(0), _cols(0)
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
				if (col_counter > _cols) { _cols = col_counter; }
			}
			if (row_counter > _rows) { _rows = row_counter; }

			// Ben test
			_dimensions = { _rows, _cols };
			_array = std::vector<T>();
			_size = _rows*_cols;
			for (auto& row : v)
			{
				for (auto& a : row)
				{
					_array.push_back(a);
				}
			}
		}

		template<class GroupingFunc>
		auto groupValues(GroupingFunc groupingFunc)
		{
			using GroupingType = std::result_of_t<GroupingFunc(T&)>;
			std::map<GroupingType, std::list<std::list<Coordinate>>> groups;
			std::vector<GroupingType> groupMapT(_size);
			std::vector<bool> visitMapT(_size);
			for (int i = 0; i < _size; i++)
			{
				groupMapT[i] = groupingFunc(_array[i]);
			}

			// start bfs
			for (int i = 0; i < _size; i++)
			{
				if (!visitMapT[i])
				{
					GroupingType groupParent = groupMapT[i];
					std::list<Coordinate> queue;	// bfs queue
					std::list<Coordinate> group;	// list of the bfs tree nodes

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
						for (int j = _dimensions.size() - 1; j >= 0; j--)
						{
							auto back = arrayLoc - dimOffset;
							auto front = arrayLoc + dimOffset;
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
					groups[groupParent].push_back(group);
				}
			}
			return groups;
		}

		/*
		template<class GroupingFunc>
		auto groupValuesOld(GroupingFunc groupingFunc)
		{
			using GroupingType = std::result_of_t<GroupingFunc(T&)>;
			std::map<GroupingType, std::list<std::list<Coordinate>>> groups;
			std::vector<std::vector<GroupingType>> groupMap(_rows, std::vector<GroupingType>(_cols));
			std::vector<std::vector<bool>> visitMap(_rows, std::vector<bool>(_cols));

			for (auto i = 0; i < _rows; i++)
			{
				for (auto j = 0; j < _cols; j++)
				{
				groupMap[i][j] = groupingFunc(_dataMatrix[i][j]);
				}
			}
			for (auto i = 0; i < _rows; i++)
			{
				for (auto j = 0; j < _cols; j++)
				{
					if (!visitMap[i][j])
					{
						// start bfs
						GroupingType groupParent = groupMap[i][j];
						std::list<Coordinate> queue;	// bfs queue
						std::list<Coordinate> group;	// list of the bfs tree nodes

						visitMap[i][j] = true;
						queue.push_back(Coordinate(i, j));
						group.push_back(Coordinate(i, j));

						while (!queue.empty())
						{
							Coordinate c = queue.front();
							queue.pop_front();
							// TODO - there is some code duplication here, try to put it in a method.
							// TODO - a long function
							// check all adjacent cells if visited and are the same type
							if (i != 0 && !visitMap[i - 1][j] && groupMap[i - 1][j] == groupParent)
							{
								queue.push_back(Coordinate(i - 1, j));
								group.push_back(Coordinate(i - 1, j));
								visitMap[i - 1][j] = true;
							}
							if (i != _rows - 1 && !visitMap[i + 1][j] && groupMap[i + 1][j] == groupParent)
							{
								queue.push_back(Coordinate(i + 1, j));
								group.push_back(Coordinate(i + 1, j));
								visitMap[i + 1][j] = true;
							}
							if (j != 0 && !visitMap[i][j - 1] && groupMap[i][j - 1] == groupParent)
							{
								queue.push_back(Coordinate(i, j - 1));
								group.push_back(Coordinate(i, j - 1));
								visitMap[i][j - 1] = true;
							}
							if (j != _cols - 1 && !visitMap[i][j + 1] && groupMap[i][j + 1] == groupParent)
							{
								queue.push_back(Coordinate(i, j + 1));
								group.push_back(Coordinate(i, j + 1));
								visitMap[i][j + 1] = true;
							}
						}
						groups[groupParent].push_back(group);
					}
				}
			}

			//ben test
			std::vector<GroupingType> groupMapT(_size);
			std::vector<bool> visitMapT(_size);
			for (int i = 0; i < _size; i++)
			{
				groupMapT[i] = groupingFunc(_array[i]);
			}
			for (int i = 0; i < _size; i++)
			{
				if (!visitMapT[i])
				{
					// start bfs
					GroupingType groupParent = groupMapT[i];
					std::list<Coordinate> queue;	// bfs queue
					std::list<Coordinate> group;	// list of the bfs tree nodes

													// get coord
					Coordinate curr(i, _dimensions);
					visitMapT[i] = true;
					queue.push_back(curr);
					group.push_back(curr);

					while (!queue.empty())
					{
						Coordinate c = queue.front();
						queue.pop_front();
						auto dimOffset = 1;
						for (int j = _dimensions.size() - 1; j >= 0; j--)
						{
							if (c[j] != 0 && !visitMapT[i - dimOffset] && groupMapT[i - dimOffset] == groupParent)
							{
								queue.push_back(c.changeDim(j, -1));
								group.push_back(c.changeDim(j, -1));
								visitMapT[i - dimOffset] = true;
							}
							if (c[j] != _dimensions[j] - 1 && !visitMapT[i + dimOffset] && groupMapT[i + dimOffset] == groupParent)
							{
								queue.push_back(c.changeDim(j, 1));
								group.push_back(c.changeDim(j, 1));
								visitMapT[i + dimOffset] = true;
							}
							dimOffset *= _dimensions[j];
						}
					}
					groups[groupParent].push_back(group);
				}
			}

			return groups;
		}
		*/
};
