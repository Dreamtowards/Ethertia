//
// Created by Dreamtowards on 2022/6/19.
//

#ifndef ETHERTIA_MODIFIERS_H
#define ETHERTIA_MODIFIERS_H

#include <vector>

#include <ethertia/util/UnifiedTypes.h>
#include <ethertia/lang/lexer/TokenType.h>

typedef u8 t_mod;

class Modifiers
{
public:

    static inline t_mod MASK_STATIC = 1 << 0,
                        MASK_CONST  = 1 << 1,
                        MASK_NATIVE = 1 << 2;

    static t_mod of(const std::vector<TokenType*>& mods) {
        t_mod code = 0;
        for (TokenType* tk : mods) {
                 if (tk == TK::STATIC) code |= MASK_STATIC;
            else if (tk == TK::CONST)  code |= MASK_CONST;
            else if (tk == TK::NATIVE) code |= MASK_NATIVE;
            else throw "unknown mod tk";
        }
        return code;
    }

    static bool isStatic(t_mod code) {
        return code & MASK_STATIC;
    }

    static bool isNative(t_mod c) {
        return c & MASK_NATIVE;
    }
};

#endif //ETHERTIA_MODIFIERS_H
