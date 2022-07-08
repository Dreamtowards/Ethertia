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

    int sizepOf(const std::string& field) {
        int p = 0;
        for (const auto& it : getSymtab()->defs) {
            if (it.first == field) {
                return p;
            }
            if (SymbolVariable* sv = dynamic_cast<SymbolVariable*>(it.second)) {
                p += sv->getType()->getTypesize();
            }
        }
        return p;
    }

};

#endif //ETHERTIA_SYMBOLCLASS_H
