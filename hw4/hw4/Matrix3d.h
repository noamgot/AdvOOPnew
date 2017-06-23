#pragma once
#include "Matrix2d.h"

template<typename T>
class Matrix3d
{
public:
	//ben test
	std::vector<T> _array;
	std::vector<int> _dimensions;
	size_t _size;

	int _rows;
	int _cols;
	int _depth;
	/*std::vector<std::vector<std::vector<T>>> _dataMatrix;*/
	
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
		
		/*_dataMatrix = std::vector<std::vector<std::vector<T>>>(_depth, std::vector<std::vector<T>>(_rows, std::vector<T>(_cols)));
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
		}*/

	
		_dimensions = { _depth, _rows, _cols };
		_array = std::vector<T>();
		_size = _rows*_cols*_depth;

		for (auto& depth : v)
		{
			for (auto& row : depth)
			{
				for (auto& col : row)
				{
					_array.push_back(col);
				}
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

};
