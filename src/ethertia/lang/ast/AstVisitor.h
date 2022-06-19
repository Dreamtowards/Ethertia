//
// Created by Dreamtowards on 2022/6/13.
//

#ifndef ETHERTIA_ASTVISITOR_H
#define ETHERTIA_ASTVISITOR_H

#include "Ast.h"
#include "AstExpr.h"
#include "AstStmt.h"
#include "AstMisc.h"

template<typename P = int>
class AstVisitor
{
public:

    void visitStatement(P p, AstStmt* a) {
             if (CAST(AstStmtBlock*))     { visitStmtBlock(p, c);     }
        else if (CAST(AstStmtBlank*))     { visitStmtBlock(p, c);     }
        else if (CAST(AstStmtIf*))        { visitStmtIf(p, c);        }
        else if (CAST(AstStmtWhile*))     { visitStmtWhile(p, c);     }
        else if (CAST(AstStmtBreak*))     { visitStmtBreak(p, c);     }
        else if (CAST(AstStmtContinue*))  { visitStmtContinue(p, c);  }
        else if (CAST(AstStmtGoto*))      { visitStmtGoto(p, c);      }
        else if (CAST(AstStmtLabel*))     { visitStmtLabel(p, c);     }
        else if (CAST(AstStmtReturn*))    { visitStmtReturn(p, c);    }
        else if (CAST(AstStmtNamespace*)) { visitStmtNamespace(p, c); }
        else if (CAST(AstStmtUsing*))     { visitStmtUsing(p, c);     }
        else if (CAST(AstStmtDefClass*))  { visitStmtDefClass(p, c);  }
        else if (CAST(AstStmtDefVar*))    { visitStmtDefVar(p, c);    }
        else if (CAST(AstStmtDefFunc*))   { visitStmtDefFunc(p, c);   }
        else if (CAST(AstStmtExpr*))      { visitStmtExpr(p, c);      }
        else { throw "Unsupported statement."; }
    }

    void visitExpression(P p, AstExpr* a) {

    }


    virtual void visitCompilationUnit(P p, AstCompilationUnit* a) {
        visitStmts(p, a->stmts);
    }

    virtual void visitStmts(P p, std::vector<AstStmt*>& stmts) {
        for (AstStmt* stmt : stmts) {
            visitStatement(p, stmt);
        }
    }

    virtual void visitStmtIf(P p, AstStmtIf* a) {
        visitExpression(p, a->cond);
        visitStatement(p, a->then);

        if (a->els) {
            visitStatement(p, a->els);
        }
    }

    virtual void visitStmtWhile(P p, AstStmtWhile* a) {
        visitExpression(p, a->cond);
        visitStatement(p, a->body);
    }

    virtual void visitStmtBlank(P p, AstStmtBlank* a) {
    }

    virtual void visitStmtBlock(P p, AstStmtBlock* a) {
        visitStmts(p, a->stmts);
    }

};

#endif //ETHERTIA_ASTVISITOR_H
