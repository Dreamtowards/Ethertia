//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOL_H
#define ETHERTIA_SYMBOL_H

#include <string>

class Symbol
{
    std::string name;

public:

    virtual std::string getQualifiedName() {
        throw "Unsupported";
    }

    std::string getSimpleName() const {
        if (name.empty())
            throw "illegal name";
        return name;
    }

    void __name(const std::string& s) {
        name = s;
    }

};

#endif //ETHERTIA_SYMBOL_H
