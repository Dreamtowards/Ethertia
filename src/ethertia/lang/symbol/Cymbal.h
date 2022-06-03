//
// Created by Dreamtowards on 2022/6/2.
//

#ifndef ETHERTIA_CYMBAL_H
#define ETHERTIA_CYMBAL_H

#include <ethertia/lang/ast/AstMisc.h>
#include <ethertia/lang/symbol/Scope.h>

#include <ethertia/lang/symbol/SymbolNamespace.h>
#include <ethertia/lang/symbol/SymbolVariable.h>
#include <ethertia/lang/symbol/SymbolFunction.h>
#include <ethertia/lang/symbol/SymbolClass.h>

class Cymbal
{
public:

    static void visitCompilationUnit(Scope* s, AstCompilationUnit* a) {
        visitStmts(s, a->stmts);
    }

    static void visitStatement(Scope* s, AstStmt* a) {
             if (CAST(AstStmtBlock*))     { visitStmtBlock(s, c);     }
        else if (CAST(AstStmtBlank*))     { }
        else if (CAST(AstStmtIf*))        { visitStmtIf(s, c);        }
        else if (CAST(AstStmtWhile*))     { visitStmtWhile(s, c);     }
        else if (CAST(AstStmtBreak*))     { }
        else if (CAST(AstStmtContinue*))  { }
        else if (CAST(AstStmtReturn*))    { visitStmtReturn(s, c);    }
        else if (CAST(AstStmtNamespace*)) { visitStmtNamespace(s, c); }
        else if (CAST(AstStmtUsing*))     { visitStmtUsing(s, c);     }
        else if (CAST(AstStmtDefClass*))  { visitStmtDefClass(s, c);  }
        else if (CAST(AstStmtDefVar*))    { visitStmtDefVar(s, c);    }
        else if (CAST(AstStmtDefFunc*))   { visitStmtDefFunc(s, c);   }
        else if (CAST(AstStmtExpr*))      { visitStmtExpr(s, c);      }
        else { throw "Unsupported statement."; }
    }

    static void visitStmts(Scope* s, const std::vector<AstStmt*>& stmts) {
        for (AstStmt* stmt : stmts) {
            visitStatement(s, stmt);
        }
    }

    static void visitStmtBlock(Scope* s, AstStmtBlock* a) {
        visitStmts(s, a->stmts);
    }

    static void visitStmtIf(Scope* s, AstStmtIf* a) {
        // cond
        visitExpression(s, a->cond);
        // then
        visitStatement(s, a->then);

        // else
        if (a->els) {
            visitStatement(s, a->els);
        }
    }

    static void visitStmtWhile(Scope* s, AstStmtWhile* a) {
        // cond
        visitExpression(s, a->cond);
        // body
        visitStatement(s, a->body);
    }

    static void visitStmtReturn(Scope* s, AstStmtReturn* a) {
        // ret expr.
        if (a->ret) {
            visitExpression(s, a->ret);
        }
    }

    static void visitStmtUsing(Scope* s, AstStmtUsing* a) {
        // type referring
        visitExpression(s, a->used);

        // decl using alias.
        s->define(a->name, a->used->getSymbol());
    }
    static void visitStmtNamespace(Scope* s, AstStmtNamespace* a) {
        // init scopes.
        Scope* lat = s;
        for (const std::string& name : AstExprMemberAccess::namesExpand(a->name)) {
            SymbolNamespace* sn = (SymbolNamespace*)lat->resolve(name);
            if (!sn) {
                sn = new SymbolNamespace(new Scope(lat));
                lat->define(name, sn);
            }
            lat = sn->getSymtab();
        }

        visitStmts(lat, a->stmts);
    }

    static void visitStmtDefClass(Scope* s, AstStmtDefClass* a) {

        s->define(a->name, new SymbolClass());

        for (AstExpr* supcl : a->superclasses) {
            visitExpression(s, supcl);
        }

        Scope* cscope = new Scope(s);
        visitStmts(cscope, a->stmts);
    }


    static void visitStmtDefVar(Scope* s, AstStmtDefVar* a) {
        // typename
        visitExpression(s, a->type);
        // decl var
        s->define(a->name, new SymbolVariable());

        // expr.
        if (a->init) {
            visitExpression(s, a->init);
        }
    }

    static void visitStmtDefFunc(Scope* s, AstStmtDefFunc* a) {
        // typename
        visitExpression(s, a->retType);
        // decl func
        s->define(a->name, new SymbolFunction());

        Scope* fscope = new Scope(s);

        // params
        for (AstStmtDefVar* param : a->params) {
            visitStmtDefVar(fscope, param);
        }
        // body
        visitStmts(fscope, a->body->stmts);
    }

    static void visitStmtExpr(Scope* s, AstStmtExpr* a) {
        visitExpression(s, a->expr);
    }






    static void visitExpression(Scope* s, AstExpr* a) {
        if (CAST(AstExprIdentifier*)) {
            a->setSymbol(s->resolve(c->name));
        } else if (CAST(AstExprLNumber*)) {
            // visitExprLNumber(s, c);  // BuiltinTypes
        } else if (CAST(AstExprLString*)) {
            // sy = ;  // ptr u16
        } else if (CAST(AstExprMemberAccess*)) {
            visitExprMemberAccess(s, c);
        } else if (CAST(AstExprFuncCall*)) {
            visitExprFuncCall(s, c);
        } else if (CAST(AstExprUnaryOp*)) {
            visitExprUnaryOp(s, c);
        } else if (CAST(AstExprBinaryOp*)) {
            visitExprBinaryOp(s, c);
        } else if (CAST(AstExprTypeCast*)) {
            // visitExprTypeCast(s, c);
        } else if (CAST(AstExprTriCond*)) {
            // visitExprTriCond(s, c);
        } else {
            throw "Unsupported expression.";
        }
    }

    static void visitExprMemberAccess(Scope* s, AstExprMemberAccess* a) {
        // lhs
        visitExpression(s, a->lhs);
        Symbol* lhs = a->lhs->getSymbol();

        if (SymbolVariable* sv = dynamic_cast<SymbolVariable*>(lhs)) {
            // DOT, object access
            // ARROW, dereference, object access.
        } else if (SymbolClass* sc = dynamic_cast<SymbolClass*>(lhs)) {
            // COLCOL, scope access
        } else if (SymbolNamespace* sn = dynamic_cast<SymbolNamespace*>(lhs)) {
            // COLCOL, scope access
        } else {
            throw "illegal lhs symbol.";
        }
    }

    static void visitExprFuncCall(Scope* s, AstExprFuncCall* a) {
        // lhs
        visitExpression(s, a->expr);
        Symbol* lhs = a->expr->getSymbol();

        if (SymbolFunction* sf = dynamic_cast<SymbolFunction*>(lhs)) {  // func call

            // sy = sf.getReturnType();
        } else if (SymbolVariable* sv = dynamic_cast<SymbolVariable*>(lhs)) {  // () operator invoke

        } else if (SymbolClass* sc = dynamic_cast<SymbolClass*>(lhs)) {  // object init.

        } else {
            throw "illegal lhs symol.";
        }
    }

    static void visitExprUnaryOp(Scope* s, AstExprUnaryOp* a) {
        visitExpression(s, a->expr);

        TokenType* typ = a->typ;
        if (a->post) {

        } else {

        }
    }

    static void visitExprBinaryOp(Scope* s, AstExprBinaryOp* a) {
        visitExpression(s, a->lhs);
        visitExpression(s, a->rhs);

        TokenType* typ = a->typ;

    }
};

#endif //ETHERTIA_CYMBAL_H
