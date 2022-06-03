//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOLSCOPED_H
#define ETHERTIA_SYMBOLSCOPED_H

#include <ethertia/lang/symbol/Scope.h>

class SymbolScoped
{
    Scope* scope = nullptr;

public:

    // Symbol Table, Scope.
    Scope* getSymtab() {
        return scope;
    }


    void _set_symtab(Scope* s) {
        scope = s;
    }
};

#endif //ETHERTIA_SYMBOLSCOPED_H
