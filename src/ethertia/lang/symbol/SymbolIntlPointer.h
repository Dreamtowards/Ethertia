//
// Created by Dreamtowards on 2022/6/13.
//

#ifndef ETHERTIA_SYMBOLINTLPOINTER_H
#define ETHERTIA_SYMBOLINTLPOINTER_H

#include "Symbol.h"
#include "TypeSymbol.h"

class SymbolIntlPointer : public Symbol, public TypeSymbol
{
public:
    static const u8 PTR_SIZE = 8;

    TypeSymbol* ptype = nullptr;

    explicit SymbolIntlPointer(TypeSymbol* ptype) : ptype(ptype) {
        __typesize(PTR_SIZE);
    }

    TypeSymbol* getPointerType() const {
        return ptype;
    }

    static SymbolIntlPointer* of(TypeSymbol* ptype) {
        return new SymbolIntlPointer(ptype);
    }

};

#endif //ETHERTIA_SYMBOLINTLPOINTER_H
