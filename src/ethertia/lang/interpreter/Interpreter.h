//
// Created by Dreamtowards on 2022/6/2.
//

#ifndef ETHERTIA_INTERPRETER_H
#define ETHERTIA_INTERPRETER_H

#include <ethertia/lang/ast/AstMisc.h>
#include "Scope.h"

class Interpreter
{
public:

    static void runCompilationUnit(Scope* s, AstCompilationUnit* a) {
        for (AstStmt* stmt : a->stmts) {
            runStatement(s, stmt);
        }
    }

    static void runStatement(Scope* s, AstStmt* a) {
        if (CAST(AstStmtNamespace*)) {         runStmtNamespace(s, c);
        } else if (CAST(AstStmtBlock*)) {      runStmtBlock(s, c);
        } else if (CAST(AstStmtDefVar*)) {     runStmtDefVar(s, c);
        } else if (CAST(AstStmtDefFunc*)) {    runStmtDefFunc(s, c);
        } else {
            throw "Unsupported Statement";
        }
    }
    static void runStmtBlock(Scope* s, AstStmtBlock* a) {
        for (AstStmt* stmt : a->stmts) {
            runStatement(s, stmt);
        }
    }

    static void runStmtNamespace(Scope* s, AstStmtNamespace* a) {
        Scope* last = s;
        for (std::string& name : AstExprMemberAccess::namesExpand(a->name)) {
            Scope* ns;
            if (ns = last->resolve(name)) {
                last = ns;
            } else {
                ns = new Scope();
                last->define(name, ns);
                last = ns;
            }
        }

        for (AstStmt* stmt : a->stmts) {
            runStatement(stmt);
        }
    }
    static void runStmtDefVar(Scope* s, AstStmtDefVar* a) {
        s->define(a->name, );
    }

};

#endif //ETHERTIA_INTERPRETER_H
