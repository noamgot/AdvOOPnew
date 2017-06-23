#pragma once
#include "Matrix2d.h"

template<typename T>
class Matrix3d
{
public:
	int _rows;
	int _cols;
	int _depth;
	std::vector<std::vector<std::vector<T>>> _dataMatrix;
	Matrix3d(std::initializer_list<std::initializer_list<std::initializer_list<T>>> v) :
	_rows(v.begin()->size()), _cols(v.begin()->end() - v.begin()->begin()), _depth(v.size())
	{
		// First, let us find the REAL dimensions of the
		auto row_counter = 0;
		auto col_counter = 0;
		auto depth_counter = 0; 
		for (auto& depth : v)
		{
			row_counter = 0;
			for (auto& row : depth)
			{
				col_counter = 0;
				for (auto& col : row)
				{
					col_counter++;
				}
				if (col_counter > _cols) { _cols = col_counter; }
				row_counter++;
			}
			if (row_counter > _rows) { _rows = row_counter; }
			depth_counter++;
		}
		if (depth_counter > _depth) { _depth = depth_counter; }
		_dataMatrix = std::vector<std::vector<std::vector<T>>>(_depth, std::vector<std::vector<T>>(_rows, std::vector<T>(_cols)));

		row_counter = 0;
		col_counter = 0;
		depth_counter = 0;

		for (auto& depth : v)
		{
			row_counter = 0;
			for (auto& row : depth)
			{
				col_counter = 0;
				for (auto& col : row)
				{
					_dataMatrix[depth_counter][row_counter][col_counter++] = col;
				}
				row_counter++;
			}
			depth_counter++;
		}
	}

	template<class GroupingFunc>
	auto groupValues(GroupingFunc groupingFunc)
	{
		using GroupingType = std::result_of_t<GroupingFunc(T&)>;
		std::map<GroupingType, std::list<std::list<Coordinate>>> groups;
		std::vector<std::vector<GroupingType>> groupMap(_rows, std::vector<GroupingType>(_cols));
		std::vector<std::vector<bool>> visitMap(_rows, std::vector<bool>(_cols));
		//GroupingType groupMap[_rows][_cols];
		//bool visitMap[_rows][_cols];

		for (auto i = 0; i < _rows; i++)
		{
			for (auto j = 0; j < _cols; j++)
			{
				groupMap[i][j] = groupingFunc(_dataMatrix[i][j]);
				//visitMap[i][j] = true;
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
		return groups;
	}
};
