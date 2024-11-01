#pragma once

template <class Container>
class DataBlockSorter
{
	static Container & sort(const Container & container);
};

template<class Container>
inline Container & DataBlockSorter<Container>::sort(const Container & container)
{
	auto container_copy = container;
	std::sort(container_copy.begin(), container_copy.end(), std::less<>{});
}
