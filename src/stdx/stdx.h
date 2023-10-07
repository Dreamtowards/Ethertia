
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

	//template<typename T, typename V>
	//T& cast(V* ptr)
	//{
	//	return *reinterpret_cast<T*>(ptr);
	//}

	template<typename T, typename V>
	T& cast(V& ptr)
	{
		return *reinterpret_cast<T*>(&ptr);
	}

	template<typename T, typename V>
	const T& cast(const V& ptr)
	{
		return *reinterpret_cast<T*>((void*)&ptr);
	}


	#pragma endregion

}