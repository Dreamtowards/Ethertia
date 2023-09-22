
#pragma once


#include <format>


namespace stdx
{

	template<typename T>
	static std::string to_string(T v) {
		std::stringstream ss;
		ss << v;
		return ss.str();
	}

	// size in bytes
	static std::string size_str(size_t _size)
	{
		static const char* _SIZES[] = { "B", "KB", "MB", "GB", "TB", "PB", "EB" };
		int i = 0;
		double size = _size;
		while (size >= 1024 && i < std::size(_SIZES) - 1)
		{
			++i;
			size /= 1024;
		}
		return std::format("{:.2f} {}", size, _SIZES[i]);
	}

	static std::string time_str(std::time_t epoch_sec = -1, const char* _fmt = "%Y-%m-%d %H:%M:%S")
	{
		if (epoch_sec == -1) epoch_sec = std::time(nullptr);

		struct tm* tm_info = std::localtime(&epoch_sec);
		assert(tm_info);

		return stdx::to_string(std::put_time(tm_info, _fmt));
	}

}