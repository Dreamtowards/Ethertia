//
// Created by Dreamtowards on 2022/6/13.
//

#ifndef ETHERTIA_SYMBOLSCANNER_H
#define ETHERTIA_SYMBOLSCANNER_H

#include <ethertia/lang/symbol/Scope.h>
#include <ethertia/lang/lexer/Lexer.h>

#include "../ast/AstVisitor.h"

class SymbolScanner
{
public:

    static void parseCompilationUnit(Scope* s, Lexer* lx) {
        while (!lx->clean_eof()) {
            parseStatement(s, lx);
        }
    }

    static void parseStatement(Scope* s, Lexer* lx) {
        TokenType* tk = lx->peek();

        if (tk == TK::NAMESPACE) {

        }
    }




};

#endif //ETHERTIA_SYMBOLSCANNER_H
