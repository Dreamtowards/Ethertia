//
// Created by Dreamtowards on 2022/5/4.
//

#ifndef ETHERTIA_STRINGS_H
#define ETHERTIA_STRINGS_H

#include <string>
#include <algorithm>

class Strings
{
public:

    static std::string erase(std::string& str, char ch) {
        str.erase(std::remove(str.begin(), str.end(), ch), str.end());
        return str;
    }

    static std::string join(const std::string& delimiter, const std::vector<std::string>& ls) {
        std::stringstream ss;
        for (const std::string& s : ls) {
            if (ss.tellp() > 0)
                ss << delimiter;
            ss << s;
        }
        return ss.str();
    }

    static std::vector<std::string> split(const std::string& str, const std::string& delimiter) {
        std::vector<std::string> ls;
        u64 beg = 0;
        u64 end;
        while ((end = str.find(delimiter, beg)) != std::string::npos) {
            ls.push_back(str.substr(beg, end-beg));
            beg = end + delimiter.length();
        }
        ls.push_back(str.substr(beg));
        return ls;
    }


    static size_t findChar(const std::string_view& str, char ch, int beg) {
        for (int i = beg; i < str.length(); ++i) {
            if (str[i] == ch)
                return i;
        }
        return std::string::npos;
    }
    static size_t findNonChar(const std::string_view& str, char ch, int beg) {
        for (int i = beg; i < str.length(); ++i) {
            if (str[i] != ch)
                return i;
        }
        return std::string::npos;
    }

    static std::vector<std::string> splitConnected(const std::string& str, char ch) {
        std::vector<std::string> ls;
        u64 beg = findNonChar(str, ch, 0);
        if (beg == std::string::npos) return {};

        u64 end;
        while ((end = findChar(str, ch, beg)) != std::string::npos) {
            ls.push_back(str.substr(beg, end-beg));
            beg = findNonChar(str, ch, end);
            if (beg == std::string::npos)
                return ls;
        }
        ls.push_back(str.substr(beg));
        return ls;
    }
};

#endif //ETHERTIA_STRINGS_H
