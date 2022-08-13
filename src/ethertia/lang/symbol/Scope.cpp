//
// Created by admin on 2022/8/13.
//

#include "Scope.h"
#include "Symbol.h"
#include "ScopedSymbol.h"
#include <vector>

Symbol* Scope::resolve(std::vector<std::string> ls) {
    Symbol* s = resolve(ls[0]);
    if (!s)
        return nullptr;

    for (int i = 1; i < ls.size(); ++i) {
        s = dynamic_cast<ScopedSymbol*>(s)->getSymtab()->findlocal(ls[i]);
        if (!s)
            return nullptr;
    }
    return s;
}