//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOLVARIABLE_H
#define ETHERTIA_SYMBOLVARIABLE_H

#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/symbol/TypeSymbol.h>

class SymbolVariable : public Symbol
{
    TypeSymbol* type = nullptr;
    bool rval = false;

public:
    SymbolVariable(const std::string& name, TypeSymbol* type) : type(type) {
        __name(name);
    }

    bool lvalue() const {
        return !rval;
    }
    bool rvalue() const {
        return rval;
    }

    TypeSymbol* getType() {
        return type;
    }

    SymbolVariable* new_rvalue() {
        return SymbolVariable::new_rvalue(type);
    }
    static SymbolVariable* new_rvalue(TypeSymbol* st) {
        auto* v = new SymbolVariable("", st);
        v->rval = true;
        return v;
    }

};

#endif //ETHERTIA_SYMBOLVARIABLE_H
