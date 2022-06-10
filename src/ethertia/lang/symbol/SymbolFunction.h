//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOLFUNCTION_H
#define ETHERTIA_SYMBOLFUNCTION_H

#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/compiler/CodeBuf.h>

class SymbolFunction : public Symbol
{
public:
    TypeSymbol* retType;

    CodeBuf codebuf{};

    SymbolFunction(const std::string& name, TypeSymbol* retType) : retType(retType) {
        __name(name);
    }

    TypeSymbol* getReturnType() {
        return retType;
    }

};

#endif //ETHERTIA_SYMBOLFUNCTION_H
