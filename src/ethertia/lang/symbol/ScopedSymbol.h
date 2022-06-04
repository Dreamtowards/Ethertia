//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SCOPEDSYMBOL_H
#define ETHERTIA_SCOPEDSYMBOL_H

#include <ethertia/lang/symbol/Scope.h>

class ScopedSymbol
{
    Scope* scope = nullptr;

public:

    // Symbol Table, Scope.
    Scope* getSymtab() {
        return scope;
    }


    void __symtab(Scope* s) {
        scope = s;
    }
};

#endif //ETHERTIA_SCOPEDSYMBOL_H
