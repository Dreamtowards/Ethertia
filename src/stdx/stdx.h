
#pragma once

#include <vector>

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
	bool exists(std::vector<T>& ls, const T& val)
	{
		return std::find(ls.begin(), ls.end(), val) != ls.end();
	}

    template<typename T>
    static T* ptr(const T& ref)
    {
        return reinterpret_cast<T*>(&ref);
    }
}