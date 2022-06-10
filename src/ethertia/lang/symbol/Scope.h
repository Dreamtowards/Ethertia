//
// Created by Dreamtowards on 2022/6/2.
//

#ifndef ETHERTIA_SCOPE_H
#define ETHERTIA_SCOPE_H

#include <map>

#include <ethertia/lang/symbol/Symbol.h>

class Scope
{
    std::map<std::string, Symbol*> defs;

    Scope* parent = nullptr;

public:
    explicit Scope(Scope* parent) : parent(parent) {}

    void define(const std::string& name, Symbol* sym) {
        assert(!findlocal(name));  // already existed.

        defs[name] = sym;
    }
    void define(Symbol* sym) {
        define(sym->getSimpleName(), sym);
    }

    Symbol* findlocal(const std::string& name) {
        auto it = defs.find(name);
        if (it == defs.end())
            return nullptr;
        return it->second;
    }
    Symbol* resolve(const std::string& name) {
        Symbol* lsym = findlocal(name);
        if (lsym)
            return lsym;
        if (parent)
            return parent->resolve(name);
        return nullptr;
    }
};

#endif //ETHERTIA_SCOPE_H
