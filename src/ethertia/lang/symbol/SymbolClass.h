//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOLCLASS_H
#define ETHERTIA_SYMBOLCLASS_H

#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/symbol/ScopedSymbol.h>
#include <ethertia/lang/symbol/TypeSymbol.h>

class SymbolClass : public Symbol, public ScopedSymbol, public TypeSymbol
{
public:

    SymbolClass(const std::string& name, Scope* cscope) {
        __name(name);
        __symtab(cscope);
    }

};

#endif //ETHERTIA_SYMBOLCLASS_H
