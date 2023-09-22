

#include "Strings.h"



std::string& Strings::erase(std::string& str, char ch) {
    str.erase(std::remove(str.begin(), str.end(), ch), str.end());
    return str;
}


std::string Strings::join(const std::string& delimiter, const std::function<std::string(int)>& fn, int n) {
    std::stringstream ss;
    for (int i = 0;i < n; ++i) {
        if (ss.tellp() > 0)
            ss << delimiter;
        ss << fn(i);
    }
    return ss.str();
}

std::string Strings::join(const std::string& delimiter, const std::vector<std::string>& ls) {
    return Strings::join(delimiter, [&](int i){ return ls[i]; }, ls.size());
}





std::string Strings::time_fmt(std::time_t epoch_sec, const char *_fmt)
{
    if (epoch_sec == -1) epoch_sec = std::time(nullptr);

    struct tm* tm_info = std::localtime(&epoch_sec);
    assert(tm_info);

    return Strings::str(std::put_time(tm_info, _fmt));
}


std::string Strings::hex(void *data, std::size_t len, bool uppercase)
{
    static char DIGIT[] = {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    std::stringstream ss;
    for (int i = 0; i < len; ++i) {
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


std::string Strings::size_str(size_t bytes)
{
    if (bytes < SIZE_KB) {
        return std::to_string(bytes) + " B";
    } else if (bytes < SIZE_MB) {
        return std::to_string((float)bytes / SIZE_KB) + " KB";
    } else if (bytes < SIZE_GB) {
        return std::to_string((float)bytes / SIZE_MB) + " MB";
    } else if (bytes < SIZE_TB) {
        return std::to_string((float)bytes / SIZE_GB) + " GB";
    } else {
        return std::to_string((float)bytes / SIZE_TB) + " TB";
    }
}