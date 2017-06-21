#include "Matrix2d.h"

template <typename T>
using deref_iter_t = std::remove_reference_t<decltype(*std::declval<T>())>;

template<class Iterator, class GroupingFunc>
auto groupValues(Iterator begin, Iterator end, GroupingFunc groupingFunc)
{
	using T = deref_iter_t<Iterator>;
	using GroupingType = std::result_of_t<GroupingFunc(T&)>;
	std::map<GroupingType, std::list<Coordinate>> groups;
	std::for_each(begin, end, [&groups, groupingFunc](const auto& val) 
	{
		groups[groupingFunc(val)].push_back(Coordinate());
	});
	
	return groups;
}