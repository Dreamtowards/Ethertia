
#pragma once


#include <format>
#include <string>

#include <functional>

namespace stdx
{

	template<typename T>
	std::string to_string(T v) {
		std::stringstream ss;
		ss << v;
		return ss.str();
	}

	std::string& erase(std::string& str, char ch);

	std::string join(std::string_view delimiter, const std::function<std::string(int)>& fn, int n);

	std::string join(std::string_view delimiter, const std::vector<std::string>& ls);


	std::string hex(void* data, size_t len = 1, bool uppercase = false);


	std::string& toupper(std::string& str);

	std::string& tolower(std::string& str);

	char at(std::string_view str, size_t pos);

    char* skip_spaces(const char* p);


	// size in bytes
	// return x.xx "B", "KB", "MB", "GB", "TB", "PB", "EB"
	std::string size_str(size_t _size);


	std::string duration(float sec, bool sec_str = true, bool apm = false);

	float duration(std::string_view str);


	inline std::string daytime(float daytime, bool sec_str = false, bool apm = true) { return stdx::duration(daytime * (24 * 60 * 60), sec_str, apm); }

	inline float daytime(std::string_view str) { return stdx::duration(str) / (24 * 60 * 60); }



	//static std::string time_str(std::time_t epoch_sec = -1, const char* _fmt = "%Y-%m-%d %H:%M:%S")
	//{
	//	if (epoch_sec == -1) epoch_sec = std::time(nullptr);
	//	struct tm* tm_info = std::localtime(&epoch_sec);
	//	assert(tm_info);
	//
	//	return stdx::to_string(std::put_time(tm_info, _fmt));
	//}

}