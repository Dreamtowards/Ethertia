//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_SYMBOLVARIABLE_H
#define ETHERTIA_SYMBOLVARIABLE_H

#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/ast/AstExpr.h>

class SymbolVariable : public Symbol
{
    AstExpr* type;

};

#endif //ETHERTIA_SYMBOLVARIABLE_H
