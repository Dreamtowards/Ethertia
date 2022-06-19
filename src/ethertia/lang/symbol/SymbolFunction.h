//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOLFUNCTION_H
#define ETHERTIA_SYMBOLFUNCTION_H

#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/compiler/CodeBuf.h>

#include <utility>

class SymbolFunction : public Symbol
{
public:
    TypeSymbol* retType;
    std::vector<SymbolVariable*> parameters;

    CodeBuf codebuf{};
    int code_spos = -1;

    SymbolFunction(const std::string& name, TypeSymbol* retType, std::vector<SymbolVariable*> parameters) : retType(retType), parameters(std::move(parameters)) {
        __name(name);
    }

    TypeSymbol* getReturnType() const {
        return retType;
    }

    [[nodiscard]] const std::vector<SymbolVariable*>& getParameters() const {
        return parameters;
    }

};

#endif //ETHERTIA_SYMBOLFUNCTION_H
