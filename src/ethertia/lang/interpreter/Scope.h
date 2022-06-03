//
// Created by Dreamtowards on 2022/6/2.
//

#ifndef ETHERTIA_SCOPE_H
#define ETHERTIA_SCOPE_H

#include <map>

class Scope
{
    std::map<std::string, int> defs;

public:

    void define() {

    }

    void* resolve(const std::string& name) {
        auto e = defs.find(name);
        if (e == defs.end())
            return nullptr;
    }
};

#endif //ETHERTIA_SCOPE_H
