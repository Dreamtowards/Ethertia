//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOLVARIABLE_H
#define ETHERTIA_SYMBOLVARIABLE_H

#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/symbol/TypeSymbol.h>
#include <ethertia/lang/symbol/Modifiers.h>

class SymbolVariable : public Symbol
{
    TypeSymbol* type = nullptr;
    bool rval = false;

public:
    t_mod mods = 0;
    u32 var_spos = -1;  // static variable only.

    int var_lpos = -1;  // local variable only. offset reside of ebp. in bytes.

public:
    SymbolVariable(const std::string& name, TypeSymbol* type, t_mod mods = 0) : type(type), mods(mods) {
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

//    static SymbolVariable* var(TypeSymbol* st) {
//        auto* v = new SymbolVariable("", st);
//        v->rval = true;
//        return v;
//    }
    static SymbolVariable* new_rvalue(TypeSymbol* st) {
        auto* v = new SymbolVariable("", st);
        v->rval = true;
        return v;
    }
    static SymbolVariable* new_lvalue(TypeSymbol* st) {
        auto* v = new SymbolVariable("", st);
        v->rval = false;
        return v;
    }

};

#endif //ETHERTIA_SYMBOLVARIABLE_H
