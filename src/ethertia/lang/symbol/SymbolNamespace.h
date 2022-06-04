//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOLNAMESPACE_H
#define ETHERTIA_SYMBOLNAMESPACE_H

#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/symbol/ScopedSymbol.h>

class SymbolNamespace : public Symbol, public ScopedSymbol
{
public:

    SymbolNamespace(const std::string& name, Scope* nscope) {
        __name(name);
        __symtab(nscope);
    }
};

#endif //ETHERTIA_SYMBOLNAMESPACE_H
