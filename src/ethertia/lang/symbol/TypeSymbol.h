//
// Created by Dreamtowards on 2022/6/4.
//

#ifndef ETHERTIA_TYPESYMBOL_H
#define ETHERTIA_TYPESYMBOL_H

#include <ethertia/util/UnifiedTypes.h>

class TypeSymbol
{
    u16 typsize = 0;

public:

    // size in bytes.
    u16 getTypesize() const {
        return typsize;
    }

    void __typesize(u16 n) {
        typsize = n;
    }
};

#endif //ETHERTIA_TYPESYMBOL_H
