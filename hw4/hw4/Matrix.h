//=======================================================
// Multi Dimensional matrix<T, dimensions>
// @author Amir Kirsh 
//=======================================================

#include <iostream>
#include <memory>
#include "Coordinate.h"
#include <initializer_list>
#include <algorithm>
#include <list>
#include <map>
#include <functional>
#include <vector>

using std::cout;
using std::endl;

template<class T, size_t DIMENSIONS>
class Matrix;

template<class T, size_t DIMENSIONS>
struct MatrixCopier {
	static void copy(T* dest, size_t dest_size, const size_t* dest_dimensions, const T* source, size_t source_size, const size_t* source_dimensions) {
		size_t dest_size0 = dest_dimensions[0] ? dest_size / dest_dimensions[0] : 0;
		size_t source_size0 = source_dimensions[0] ? source_size / source_dimensions[0] : 0;
		for (size_t i = 0; i < source_dimensions[0]; ++i) {
			MatrixCopier<T, DIMENSIONS - 1>::copy(dest + (i * dest_size0), dest_size0, dest_dimensions + 1, source + (i * source_size0), source_size0, source_dimensions + 1);
		}
	}
};

template<class T>
struct MatrixCopier<T, 1> {
	static void copy(T* dest, size_t dest_size, const size_t* dest_dimensions, const T* source, size_t source_size, const size_t* source_dimensions) {
		for (size_t i = 0; i < source_size; ++i) {
			dest[i] = source[i];
		}
	}
};

template<class T, size_t DIMENSIONS>
struct MatrixPrinter {
	static void print(const T* values, size_t size, const size_t* dimensions, std::ostream& out = cout) {
		out << '{';
		size_t size0 = size / dimensions[0];
		MatrixPrinter<T, DIMENSIONS - 1>::print(values, size0, dimensions + 1, out);
		for (size_t i = 1; i < dimensions[0]; ++i) {
			out << ',';
			MatrixPrinter<T, DIMENSIONS - 1>::print(values + (i*size0), size0, dimensions + 1, out);
		}
		out << '}';
	}
};

template<class T>
struct MatrixPrinter<T, 1> {
	static void print(const T* values, size_t size, const size_t* dimensions, std::ostream& out = cout) {
		out << '{';
		out << values[0];
		for (size_t i = 1; i < size; ++i) {
			out << ',' << values[i];
		}
		out << '}';
	}
};

template<class T, size_t DIMENSIONS>
class Matrix {
	constexpr static size_t NUM_DIMENSIONS = DIMENSIONS;
	std::unique_ptr<T[]> _array = nullptr;
	size_t _dimensions[DIMENSIONS] = {};
	const size_t _size = 0;
	friend class Matrix<T, DIMENSIONS + 1>;
public:
	size_t size() const { return _size; }
	Matrix() {}

	// DIMENSIONS == 1
	// We want here a ctor with this signature:
	//    Matrix(const std::initializer_list<T>& values) {
	// but SFINAE is needed to block this ctor from being used by Matrix with DIMENSIONS > 1
	// The SFINAE results with the exact signature we want, but only for cases DIMENSIONS == 1
	template<typename G = T>
	Matrix(const std::initializer_list<typename std::enable_if_t<DIMENSIONS == 1, G>>& values) {
		const_cast<size_t&>(_size) = values.size();
		_dimensions[0] = _size;
		_array = std::make_unique<T[]>(_size);
		size_t i = 0;
		for (auto& val : values) {
			_array[i++] = val;
		}
	}
	// DIMENSIONS > 1
	// We want here a ctor with this signature:
	//    Matrix(const std::initializer_list<Matrix<T, DIMENSIONS-1>>& values) {
	// although this ctor is not useful and won't be used by Matrix<T, 1> it will still be in class
	// and thus would compile as part of Matrix<T, 1> resulting with a parameter of type:
	//          const std::initializer_list< Matrix<T, 0> >& values
	// having Matrix<T, 0> as a parameter - even for a function that is not in use, inside a class that is used
	// would require the compiler to instantiate Matrix<T, 0> class which results with a warning
	// the SFINAE below solves this warning.
	// The SFINAE results with the exact signature we want, but only for cases DIMENSIONS > 1
	template<typename G = T>
	Matrix(const std::initializer_list<Matrix<typename std::enable_if_t<(DIMENSIONS > 1), G>, DIMENSIONS - 1>>& values) {
		_dimensions[0] = values.size();
		for (auto& m : values) {
			for (size_t dim = 0; dim < DIMENSIONS - 1; ++dim) {
				if (m._dimensions[dim] > _dimensions[dim + 1]) {
					_dimensions[dim + 1] = m._dimensions[dim];
				}
			}
		}
		size_t size = 1;
		for (size_t dim = 0; dim < DIMENSIONS; ++dim) {
			size *= _dimensions[dim];
		}

		const_cast<size_t&>(_size) = size;
		_array = std::make_unique<T[]>(_size); // "zero initialized" - T()
		size_t i = 0;
		size_t dest_size = _size / _dimensions[0];
		for (auto& m : values) {
			MatrixCopier<T, DIMENSIONS - 1>::copy(&(_array[i * dest_size]), dest_size, _dimensions + 1, m._array.get(), m._size, m._dimensions);
			++i;
		}
	}

	Matrix(Matrix&& m) {
		*this = std::move(m);
	}

	auto& operator=(Matrix&& m) {
		std::swap(_array, m._array);
		std::swap(const_cast<size_t&>(_size), const_cast<size_t&>(m._size));
		std::swap(_dimensions, m._dimensions);
		return *this;
	}

	size_t getDimension(size_t i) const {
		return _dimensions[i];
	}

	friend std::ostream& operator<<(std::ostream& out, const Matrix& m) {
		MatrixPrinter<T, DIMENSIONS>::print(m._array.get(), m._size, m._dimensions, out);
		return out;
	}

	template<class GroupingFunc>
	auto groupValues(GroupingFunc groupingFunc)
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

				Coordinate curr(i, _dimensions, DIMENSIONS); // get coord
				visitMapT[i] = true;
				queue.push_back(curr);
				group.push_back(curr);

				while (!queue.empty())
				{
					Coordinate c = queue.front();
					queue.pop_front();
					auto arrayLoc = c.getMatloc();
					auto dimOffset = 1;
					for (int j = DIMENSIONS - 1; j >= 0; j--)
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

};


// defining Matrix2d<T> as Matrix<T, 2>
template<class T>
using Matrix2d = Matrix<T, 2>;

// defining Matrix3d<T> as Matrix<T, 3>
template<class T>
using Matrix3d = Matrix<T, 3>;

// defining IntVector as Matrix<int, 1>
using IntVector = Matrix<int, 1>;



