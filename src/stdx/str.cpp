
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









// safe access. overflow -> return 0. useful for check { str[x] == '?' }
char stdx::at(std::string_view str, size_t pos) {
	if (pos < str.length())
		return str[pos];
	return 0;
}


std::string stdx::daytime(float daytime, bool apm, bool gen_sec)
{
	float hr = daytime * 24.0;
	float mn = (hr - std::floor(hr)) * 60.0;
	float sec = (mn - std::floor(mn)) * 60.0;
	std::stringstream ss;
	bool am = hr < 12;

	ss << std::format("{:02}:{:02}", (int)(am ? hr : hr - 12), (int)mn);

	if (gen_sec)
	{
		ss << std::format(":{:05.2f}", sec);
	}

	if (apm)
	{
		ss << " " << (am ? "AM" : "PM");
	}
	return ss.str();
}

float stdx::daytime(std::string_view _str)
{
	char* str = (char*)_str.data();

	float hr = std::strtof(str, &str);

	float mn = 0;
	if (*str == ':') {
		++str;
		mn = std::strtof(str, &str);
	}

	float sec = 0;
	if (*str == ':') {
		++str;
		sec = std::strtof(str, &str);
	}

	if (*str == 'P' || *str == 'p') {  // PM
		hr += 12.0f;
	}
	
	float t = hr / 24.0;
	if (mn) t += mn / (60.0 * 24.0);
	if (sec) t += sec / (60.0 * 60.0 * 24.0);
	return t;
}