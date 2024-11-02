#pragma once

#include <algorithm>

template <class Container>
class DataBlockSorter
{
	void operator()(Container & container);
};

template<class Container>
inline void DataBlockSorter<Container>::operator()(Container & container)
{
	std::sort(container.begin(), container.end(), std::less<>{});
}
