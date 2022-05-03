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

    static std::string erase(const std::string& str, int ch) {
        str.erase(std::remove(str.begin(), str.end(), ch), str.end());
    }
};

#endif //ETHERTIA_STRINGS_H
