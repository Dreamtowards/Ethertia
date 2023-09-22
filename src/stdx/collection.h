

#pragma once


#include <vector>
#include <map>
#include <string>


namespace stdx
{

	// return true: success erased.
	template<typename T>
	bool erase(std::vector<T>& ls, size_t idx)
	{
		auto old = ls.size();
		ls.erase(ls.begin() + idx);
		return ls.size() != old;
	}

	template<typename T>
	bool erase(std::vector<T>& ls, const T& val)
	{
		auto it = std::find(ls.begin(), ls.end(), val);
		if (it == ls.end())
			return false;
		ls.erase(it);
		return true;
	}

	template<typename T>
	bool exists(const std::vector<T>& ls, const T& val)
	{
		return std::find(ls.begin(), ls.end(), val) != ls.end();
	}

	template<typename K, typename V>
	static K find_key(const std::map<K, V>& m, const V& val)
	{
		for (auto it = m.cbegin(); it != m.cend(); ++it)
		{
			if (it->second == val)
			{
				return it->first;
			}
		}
		throw std::runtime_error("stdx::find_key error: no such value");
	}
}