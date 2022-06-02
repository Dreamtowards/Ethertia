//
// Created by Dreamtowards on 2022/5/25.
//

#ifndef ETHERTIA_ASTMISC_H
#define ETHERTIA_ASTMISC_H

#include <utility>

#include "Ast.h"
#include "AstStmt.h"

class AstCompilationUnit : public Ast
{
public:
    std::vector<AstStmt*> stmts;

    AstCompilationUnit(std::vector<AstStmt*> stmts) : stmts(std::move(stmts)) {}
};

class AstAttribute : public Ast
{
public:
    AstExprFuncCall* cons;

    AstAttribute(AstExprFuncCall *cons) : cons(cons) {}
};

class AstModifiers : public Ast
{
public:
    std::vector<TokenType*> modifiers;

    AstModifiers(std::vector<TokenType*> modifiers) : modifiers(std::move(modifiers)) {}
};

#endif //ETHERTIA_ASTMISC_H
