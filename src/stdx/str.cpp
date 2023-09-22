
#include "str.h"

#include <sstream>


std::string& stdx::erase(std::string& str, char ch)
{
	str.erase(std::remove(str.begin(), str.end(), ch), str.end());
	return str;
}

std::string stdx::join(std::string_view delimiter, const std::function<std::string(int)>& fn, int n)
{
	std::stringstream ss;
	for (int i = 0; i < n; ++i)
	{
		if (ss.tellp() > 0)
			ss << delimiter;
		ss << fn(i);
	}
	return ss.str();
}

std::string stdx::join(std::string_view delimiter, const std::vector<std::string>& ls)
{
	return stdx::join(delimiter, [&](int i) { return ls[i]; }, ls.size());
}


std::string stdx::hex(void* data, std::size_t len, bool uppercase)
{
	static char DIGIT[] = {
			'0', '1', '2', '3', '4', '5', '6', '7',
			'8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
	};

	std::stringstream ss;
	for (int i = 0; i < len; ++i) 
	{
		uint8_t b = ((uint8_t*)data)[i];

		int b0 = (b >> 4) & 0x0F;
		int b1 = b & 0x0F;
		if (uppercase) {
			ss << std::toupper(DIGIT[b0]) << std::toupper(DIGIT[b1]);
		} else {
			ss << DIGIT[b0] << DIGIT[b1];
		}
	}
	return ss.str();
}




static std::string& stdx::toupper(std::string& str) {
	for (auto& c : str) c = std::toupper(c);
	return str;
}

static std::string& stdx::tolower(std::string& str) {
	for (auto& c : str) c = std::tolower(c);
	return str;
}





std::string stdx::size_str(size_t _size)
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