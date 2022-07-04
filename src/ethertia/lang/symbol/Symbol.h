//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOL_H
#define ETHERTIA_SYMBOL_H

#include <string>

class Symbol
{
    std::string name;
    std::string qualname;

public:

    virtual void _makev() {};

    std::string getQualifiedName() const {
        if (qualname.empty())
            throw "illegal qualified name";
        return qualname;
    }

    std::string getSimpleName() const {
        if (name.empty())
            throw "illegal name";
        return name;
    }

    void __name(const std::string& s) {
        name = s;
    }
    void __qualname(const std::string& s) {
        qualname = s;
    }


};

#endif //ETHERTIA_SYMBOL_H
