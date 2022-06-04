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
    CodeBuf codebuf;

    SymbolFunction(const std::string& name) {
        __name(name);
    }
};

#endif //ETHERTIA_SYMBOLFUNCTION_H
