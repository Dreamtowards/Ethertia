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
#include <ethertia/lang/symbol/SymbolInternalTypes.h>

class Cymbal
{
public:
    inline static std::map<std::string, AstStmtDefFunc*> functions;

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
                sn = new SymbolNamespace(name, new Scope(lat));
                lat->define(sn);
            }
            lat = sn->getSymtab();
        }

        visitStmts(lat, a->stmts);
    }

    static void visitStmtDefClass(Scope* s, AstStmtDefClass* a) {

        s->define(new SymbolClass(a->name));

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
        s->define(a->vsymbol = new SymbolVariable(a->name, a->type->getSymbolType()));

        // expr.
        if (a->init) {
            visitExpression(s, a->init);
        }
    }

    static void visitStmtDefFunc(Scope* s, AstStmtDefFunc* a) {
        // typename
        visitExpression(s, a->retType);
        // decl func
        SymbolFunction* fsymbol = new SymbolFunction(a->name);
        s->define(fsymbol);
        a->fsymbol = fsymbol;

        Scope* fscope = new Scope(s);

        // params
        for (AstStmtDefVar* param : a->params) {
            visitStmtDefVar(fscope, param);
        }
        // body
        visitStmts(fscope, a->body->stmts);

        Cymbal::functions[fsymbol->getSimpleName()] = a;
    }

    static void visitStmtExpr(Scope* s, AstStmtExpr* a) {
        visitExpression(s, a->expr);
    }






    static void visitExpression(Scope* s, AstExpr* a) {
             if (CAST(AstExprIdentifier*))   { visitExprIdentifier(s, c); }
        else if (CAST(AstExprLNumber*))      { visitExprLNumber(s, c);    }
        else if (CAST(AstExprLString*))      { }
        else if (CAST(AstExprMemberAccess*)) { visitExprMemberAccess(s, c);}
        else if (CAST(AstExprFuncCall*))     { visitExprFuncCall(s, c); }
        else if (CAST(AstExprUnaryOp*))      { visitExprUnaryOp(s, c); }
        else if (CAST(AstExprBinaryOp*))     { visitExprBinaryOp(s, c); }
        else if (CAST(AstExprTypeCast*))     { /* visitExprTypeCast(s, c); */ }
        else if (CAST(AstExprTriCond*))      { /* visitExprTriCond(s, c);*/ }
        else { throw "Unsupported expression."; }
    }

    static void visitExprIdentifier(Scope* s, AstExprIdentifier* a) {
        Symbol* sym = s->resolve(a->name);
        if (!sym)
            throw "Couldn't found such symbol";
        a->setSymbol(sym);
    }

    static void visitExprLNumber(Scope* s, AstExprLNumber* a) {
        TokenType* typ = a->typ;

        TypeSymbol* st = nullptr;
             if (typ == TK::L_I32) { st = &SymbolInternalTypes::I32; }
        else if (typ == TK::L_I64) { st = &SymbolInternalTypes::I64; }
        else { throw "Unsupported literal number."; }

        a->setSymbol(SymbolVariable::new_rvalue(st));
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
        SymbolVariable* lhss = a->lhs->getSymbolVar();
        SymbolVariable* rhss = a->rhs->getSymbolVar();

        TokenType* typ = a->typ;
        if (typ == TK::EQ) {
            assert(lhss->lvalue());  // lhs must be lvalue.
            assert(lhss->getType()->getTypesize() == rhss->getType()->getTypesize());

            a->setSymbol(lhss);
        } else if (typ == TK::PLUS) {
            assert(lhss->getType() == rhss->getType());

            a->setSymbol(SymbolVariable::new_rvalue(lhss->getType()));
        }
    }
};

#endif //ETHERTIA_CYMBAL_H
