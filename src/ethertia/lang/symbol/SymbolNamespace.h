//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOLNAMESPACE_H
#define ETHERTIA_SYMBOLNAMESPACE_H

#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/symbol/SymbolScoped.h>

class SymbolNamespace : public Symbol, public SymbolScoped
{
public:

    SymbolNamespace(Scope* nscope) {
        _set_symtab(nscope);
    }
};

#endif //ETHERTIA_SYMBOLNAMESPACE_H
