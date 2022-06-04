//
// Created by Dreamtowards on 2022/6/4.
//

#ifndef ETHERTIA_SYMBOLINTERNALTYPES_H
#define ETHERTIA_SYMBOLINTERNALTYPES_H

#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/symbol/TypeSymbol.h>
#include <ethertia/util/UnifiedTypes.h>
#include "Scope.h"

class SymbolInternalTypes : public Symbol, public TypeSymbol
{
public:
    static SymbolInternalTypes VOID,
                               U8,
                               U16,
                               U32,
                               U64,
                               I8,
                               I16,
                               I32,
                               I64,
                               F32,
                               F64;

    explicit SymbolInternalTypes(const std::string& name, u16 sz) {
        __name(name);
        __typesize(sz);
    }

    static void initInternalTypes(Scope* s) {

        s->define(&VOID);

        s->define(&U8);
        s->define(&U16);
        s->define(&U32);
        s->define(&U64);

        s->define(&I8);
        s->define(&I16);
        s->define(&I32);
        s->define(&I64);

        s->define(&F32);
        s->define(&F64);
    }


};

#endif //ETHERTIA_SYMBOLINTERNALTYPES_H
