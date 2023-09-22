
#pragma once

//#include "collection.h"
//#include "str.h"

namespace stdx
{

	#pragma region common core

	template<typename T>
	T* ptr(const T& ref)
	{
		return reinterpret_cast<T*>(&ref);
	}

	#pragma endregion

}