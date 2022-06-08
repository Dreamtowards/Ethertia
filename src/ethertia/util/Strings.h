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
};

#endif //ETHERTIA_STRINGS_H
