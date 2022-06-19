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
#include <ethertia/lang/symbol/SymbolIntlPointer.h>
#include <ethertia/lang/compiler/CodeGen.h>
#include <ethertia/lang/machine/Macedure.h>

class Cymbal
{
public:
//    inline static std::map<std::string, AstStmtDefFunc*> functions;

    // static write pos.
    inline static u32 spos = 0;
    inline static u8 sbuf[2048];

    static void visitCompilationUnit(Scope* s, AstCompilationUnit* a) {
        visitStmts(s, a->stmts);
    }

    static void visitAttribute(Scope* s, AstAttribute* a) {
        AstExpr* expr = a->expr;
        visitExpression(s, expr);

        Symbol* sym = expr->getSymbol();
        if (SymbolClass* sc = dynamic_cast<SymbolClass*>(sym)) {

        } else if (SymbolFunction* sf = dynamic_cast<SymbolFunction*>(sym)) {

        } else {
            throw "Illegal attribute symbol";
        }
    }

    static void visitStatement(Scope* s, AstStmt* a) {
             if (CAST(AstStmtBlock*))     { visitStmtBlock(s, c);     }
        else if (CAST(AstStmtBlank*))     { }
        else if (CAST(AstStmtIf*))        { visitStmtIf(s, c);        }
        else if (CAST(AstStmtWhile*))     { visitStmtWhile(s, c);     }
        else if (CAST(AstStmtBreak*))     { }  // validate enclosing loop.
        else if (CAST(AstStmtContinue*))  { }
        else if (CAST(AstStmtGoto*))      { }  // check on CodeGen. labels have no their symbol-table.
        else if (CAST(AstStmtLabel*))     { }
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
        Scope* bscope = new Scope(s);

        visitStmts(bscope, a->stmts);

//        if (currFunction)  // inside a function.
//        {
//            int old_localvp = currLocalvarp;
//
//
//            currLocalvarp = old_localvp;
//        }
//        else
//        {
//            visitStmts(s, a->stmts);
//        }
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
        // TypeSymbol* funcRet = s->findEnclosingFunction().getReturnType();

        // ret expr.
        if (a->ret) {
            visitExpression(s, a->ret);

            // assert(funcRet == a->ret->getSymbolVar()->getType());
        } else {
            // assert(funcRet == VOID);
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
            SymbolNamespace* sn = (SymbolNamespace*)lat->findlocal(name);
            if (!sn) {
                sn = new SymbolNamespace(name, new Scope(lat));
                lat->define(sn);
            }
            lat = sn->getSymtab();
        }

        visitStmts(lat, a->stmts);
    }

    static void visitStmtDefClass(Scope* s, AstStmtDefClass* a) {

        Scope* cscope = new Scope(s);
        s->define(new SymbolClass(a->name, cscope));

        for (AstExpr* supcl : a->superclasses) {
            visitExpression(s, supcl);
        }

        visitStmts(cscope, a->stmts);
    }

//    inline static int currLocalvarp = 0;
//    inline static SymbolFunction* currFunction = nullptr;

    static void visitStmtDefVar(Scope* s, AstStmtDefVar* a) {
        // typename
        visitExpression(s, a->type);
        // decl var
        SymbolVariable* vsymbol = new SymbolVariable(a->name, a->type->getSymbolType(), Modifiers::of(a->mods->modifiers));
        s->define(vsymbol);
        a->vsymbol = vsymbol;

        if (Modifiers::isStatic(vsymbol->mods)) {
            vsymbol->var_spos = spos;
            spos += vsymbol->getType()->getTypesize();
        }

//        vsymbol->localpos = currLocalvarp;
//        currLocalvarp += vsymbol->getType()->getTypesize();

        // expr.
        if (a->init) {
            visitExpression(s, a->init);
        }
    }

    static void visitStmtDefFunc(Scope* s, AstStmtDefFunc* a) {
        // typename
        visitExpression(s, a->retType);

        Scope* fscope = new Scope(s);

        // params
        std::vector<SymbolVariable*> params;
        for (AstStmtDefVar* param : a->params) {
            visitStmtDefVar(fscope, param);
            params.push_back(param->vsymbol);
        }

        // decl func
        SymbolFunction* fsymbol = new SymbolFunction(a->name, a->retType->getSymbolType(), params);
        s->define(fsymbol);
        a->fsymbol = fsymbol;
//                SymbolFunction* oldfunc = currFunction;
//                currFunction = fsymbol;

        // body
        visitStmtBlock(fscope, a->body);  // StmtBlock Dependency. for localvar.

//                currFunction = oldfunc;
//        Cymbal::functions[fsymbol->getSimpleName()] = a;


        CodeBuf* cbuf = &fsymbol->codebuf;
        CodeGen::visitStmtDefFunc(cbuf, a);

        fsymbol->code_spos = spos;
        int len = cbuf->bufpos();
        memcpy(&sbuf[spos], cbuf->bufptr(0), len);
        spos += len;
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
        else if (CAST(AstExprTypeCast*))     { visitExprTypeCast(s, c); }
        else if (CAST(AstExprTriCond*))      { /* visitExprTriCond(s, c);*/ }
        else if (CAST(AstExprSizeOf*))       { visitExprSizeOf(s, c); }
        else { throw "Unsupported expression."; }
    }

    static void visitExprIdentifier(Scope* s, AstExprIdentifier* a) {
        Symbol* sym = s->resolve(a->name);
        if (!sym)
            throw "Couldn't found such symbol";
        a->setSymbol(sym);
    }

    static void visitExprSizeOf(Scope* s, AstExprSizeOf* a) {
        visitExpression(s, a->expr);

        a->setSymbol(SymbolVariable::new_rvalue(&SymbolInternalTypes::U32));
    }

    static void visitExprTypeCast(Scope* s, AstExprTypeCast* a) {
        visitExpression(s, a->expr);
        visitExpression(s, a->type);

        a->setSymbol(SymbolVariable::new_rvalue(a->type->getSymbolType()));
    }

    static void visitExprLNumber(Scope* s, AstExprLNumber* a) {
        TokenType* typ = a->typ;

        TypeSymbol* st = nullptr;
             if (typ == TK::L_I32) { st = &SymbolInternalTypes::I32; }
        else if (typ == TK::L_I64) { st = &SymbolInternalTypes::I64; }
        else if (typ == TK::TRUE)  { st = &SymbolInternalTypes::BOOL; }
        else if (typ == TK::FALSE) { st = &SymbolInternalTypes::BOOL; }
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
            throw "unsupp";
        } else if (SymbolClass* sc = dynamic_cast<SymbolClass*>(lhs)) {
            // COLCOL, scope access
            throw "unsupp";
        } else if (SymbolNamespace* sn = dynamic_cast<SymbolNamespace*>(lhs)) {
            // COLCOL, scope access
            throw "unsupp";
        } else {
            throw "illegal lhs symbol.";
        }
    }

    static void visitExprFuncCall(Scope* s, AstExprFuncCall* a) {
        // lhs
        visitExpression(s, a->expr);
        Symbol* lhs = a->expr->getSymbol();

        // args
        for (AstExpr* arg : a->args) {
            visitExpression(s, arg);
        }

        if (SymbolFunction* sf = dynamic_cast<SymbolFunction*>(lhs)) {  // func call
            assert(a->args.size() == sf->getParameters().size());
            for (int i = 0; i < a->args.size(); ++i) {
                assert(a->args[i]->getSymbolVar()->getType() == sf->getParameters()[i]->getType());
            }

            a->setSymbol(SymbolVariable::new_rvalue(sf->getReturnType()));
        } else if (SymbolClass* sc = dynamic_cast<SymbolClass*>(lhs)) {  // object init.

            throw "unsupp ctor";
        } else if (SymbolVariable* sv = dynamic_cast<SymbolVariable*>(lhs)) {  // () operator invoke

            throw "unsupp oper overload";
        } else {
            throw "illegal lhs symol.";
        }
    }

    static void visitExprUnaryOp(Scope* s, AstExprUnaryOp* a) {
        visitExpression(s, a->expr);

        TokenType* typ = a->typ;
        if (a->post) {
            if (typ == TK::STAR) {
                a->setSymbol(SymbolIntlPointer::of(a->expr->getSymbolType()));
            } else {
                throw "Unsupported post unary";
            }
        } else {
            if (typ == TK::AMP) {
                SymbolVariable* sv = a->expr->getSymbolVar();
                assert(sv->lvalue());
                a->setSymbol(SymbolVariable::new_rvalue(SymbolIntlPointer::of(sv->getType())));
            } else if (typ == TK::STAR) {
                SymbolVariable* sv = a->expr->getSymbolVar();
                SymbolIntlPointer* sp = (SymbolIntlPointer*)sv->getType();
                a->setSymbol(SymbolVariable::new_lvalue(sp->getPointerType()));
            } else {
                throw "unsupp pre unary";
            }
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
        } else if (typ == TK::GT || typ == TK::LT) {

            a->setSymbol(SymbolVariable::new_rvalue(&SymbolInternalTypes::U8));
        }
    }
};

#endif //ETHERTIA_CYMBAL_H
