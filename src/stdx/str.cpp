
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


char* stdx::skip_spaces(const char* p)
{
	while (*p && std::isspace(*p))
	{
		++p;
	}	
	return (char*)p;
}





//Log::info("{}", stdx::daytime(stdx::daytime("5"))); 
//Log::info("{}", stdx::daytime(stdx::daytime("00:30.5pm")));
//Log::info("{}", stdx::daytime(stdx::daytime("10:30.5:2.59 am")));
//Log::info("{}", stdx::daytime(stdx::daytime("::60.5p")));
//Log::info("{}", stdx::daytime(stdx::daytime("  100  :  :  10.9  Pm ")));
// 
//[2023-09-22 12:47:00.9314761][30848/INFO][Ethertia.cpp:45]: 05:00
//[2023-09-22 12:47:00.9328794][30848/INFO][Ethertia.cpp:46]: 00:30:30.00
//[2023-09-22 12:47:00.9336121][30848/INFO][Ethertia.cpp:47]: 10:30:32.59
//[2023-09-22 12:47:00.9341415][30848/INFO][Ethertia.cpp:48]: 00:01:00.50
//[2023-09-22 12:47:00.9344923][30848/INFO][Ethertia.cpp:49]: 100:00:10.90


std::string stdx::duration(float sec, bool sec_str, bool apm)
{
	int mn = (int)std::floor(sec / 60.0f) % 60;
	int hr = (int)std::floor(sec / (60 * 60.0f));
	sec = ((int)sec % 60) + (sec - (int)sec);

	std::stringstream ss;
	bool am = hr < 12;

	ss << std::format("{:02}:{:02}", ((apm && !am) ? hr-12 : hr), mn);

	if (sec_str) // && sec != 0)
	{
		ss << std::format(":{:05.2f}", sec);
	}

	if (apm)
	{
		ss << " " << (am ? "AM" : "PM");
	}
	return ss.str();
}

float stdx::duration(std::string_view _str)
{
	char* str = (char*)_str.data();

	str = stdx::skip_spaces(str);
	float hr = std::strtof(str, &str);

	str = stdx::skip_spaces(str);
	float mn = 0;
	if (*str == ':') {
		++str;
		str = stdx::skip_spaces(str);
		mn = std::strtof(str, &str);
	}

	str = stdx::skip_spaces(str);
	float sec = 0;
	if (*str == ':') {
		++str;
		str = stdx::skip_spaces(str);
		sec = std::strtof(str, &str);
	}

	str = stdx::skip_spaces(str);
	if (*str == 'P' || *str == 'p') {  // PM
		hr += 12.0f;
	}
	
	float t = hr / 24.0;
	if (mn) t += mn / (60.0 * 24.0);
	if (sec) t += sec / (60.0 * 60.0 * 24.0);
	return t;
}