#pragma once
#include <initializer_list>
#include <iostream>
#include <algorithm>
#include <list>
#include <map>
#include <functional>
#include "Coordinate.h"

template<typename T, size_t ROWS, size_t COLS>
class Matrix2d 
{
	int rows = ROWS;
	int cols = COLS;
	T arr[ROWS*COLS] = {};
	Matrix2d(std::initializer_list<std::initializer_list<T>> v) 
	{
		int i = 0;
		for (auto& row : v) 
		{
			for (auto& a : row) 
			{
				arr[i++] = a;
			}
		}
	}
	class Row 
	{
		T* _row;
	public:
		Row(T* row) : _row(row) {}
		const T* begin() const 
		{
			return _row;
		}
		const T* end() const 
		{
			return _row + COLS;
		}
	};
	class iterator 
	{
		T* _row;
	public:
		iterator(T* row) : _row(row) {}
		iterator operator++() 
		{
			_row = _row + COLS;
			return *this;
		}
		Row operator*() 
		{
			return _row;
		}
		bool operator!=(iterator other) 
		{
			return _row != other._row;
		}
	};
	const iterator begin()
	{
		return arr;
	}
	const iterator end() 
	{
		return arr + COLS * ROWS;
	}

	template<class Iterator, class GroupingFunc>
	auto groupValues(Iterator begin, Iterator end, GroupingFunc groupingFunc);
};