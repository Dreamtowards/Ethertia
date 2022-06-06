//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_CODEGEN_H
#define ETHERTIA_CODEGEN_H

#include <ethertia/lang/compiler/CodeBuf.h>
#include <ethertia/lang/ast/AstMisc.h>
#include <ethertia/lang/symbol/Scope.h>

class CodeGen
{
public:
    static const u8 PTR_SIZE = 4;


    static void visitStmtDefFunc(CodeBuf* cbuf, AstStmtDefFunc* a) {

        for (AstStmtDefVar* param : a->params) {
            visitStmtDefVar(cbuf, param);
        }

        visitStmts(cbuf, a->body->stmts);

        cbuf->_nop();
    }

    static void visitStmtDefVar(CodeBuf* cbuf, AstStmtDefVar* a) {  cbuf->_verbo(a->str_v());
        cbuf->defvar(a->vsymbol);

        if (a->init) {
            AstExpr* expr = a->init;
            // assign
            cbuf->_ldl(a->vsymbol);
            mov(cbuf, expr);
        }
    }

    static void mov(CodeBuf* cbuf, AstExpr* expr) {
        visitExpression(cbuf, expr);  // src ptr.

        SymbolVariable* sv = expr->getSymbolVar();
        u8 tsize = sv->getType()->getTypesize();
        if (sv->rvalue()) {
            cbuf->_mov_pop(tsize);
        } else {
            cbuf->_mov(tsize);
        }
    }

    static void visitStmtExpr(CodeBuf* cbuf, AstStmtExpr* a) {  cbuf->_verbo(a->str_v());
        visitExpression(cbuf, a->expr);
        cbuf->_pop(a->expr->getSymbolVar()->getType()->getTypesize());
    }


    static void visitStmts(CodeBuf* cbuf, const std::vector<AstStmt*>& stmts) {
        for (AstStmt* stmt : stmts) {
            visitStatement(cbuf, stmt);
        }
    }

    static void visitStatement(CodeBuf* cbuf, AstStmt* a) {
             if (CAST(AstStmtBlock*))     { visitStmts(cbuf, c->stmts);  }
//        else if (CAST(AstStmtIf*))        { visitStmtIf(cbuf, c);        }
//        else if (CAST(AstStmtWhile*))     { visitStmtWhile(cbuf, c);     }
        else if (CAST(AstStmtBreak*))     { }
        else if (CAST(AstStmtContinue*))  { }
//        else if (CAST(AstStmtReturn*))    { visitStmtReturn(cbuf, c);    }
        else if (CAST(AstStmtDefVar*))    { visitStmtDefVar(cbuf, c);    }
        else if (CAST(AstStmtExpr*))      { visitStmtExpr(cbuf, c);      }
        else { throw "Unsupported statement."; }
    }


    static void visitExpression(CodeBuf* cbuf, AstExpr* a) {
             if (CAST(AstExprIdentifier*))   { visitExprIdentifier(cbuf, c); }
        else if (CAST(AstExprLNumber*))      { visitExprLNumber(cbuf, c);    }
        else if (CAST(AstExprLString*))      { throw "unsupp"; }
//        else if (CAST(AstExprMemberAccess*)) { visitExprMemberAccess(cbuf, c);}
//        else if (CAST(AstExprFuncCall*))     { visitExprFuncCall(s, c); }
//        else if (CAST(AstExprUnaryOp*))      { visitExprUnaryOp(s, c); }
        else if (CAST(AstExprBinaryOp*))     { visitExprBinaryOp(cbuf, c); }
        else if (CAST(AstExprTypeCast*))     { /* visitExprTypeCast(s, c); */ }
        else if (CAST(AstExprTriCond*))      { /* visitExprTriCond(s, c);*/ }
        else { throw "Unsupported expression."; }
    }

    static void visitExprIdentifier(CodeBuf* cbuf, AstExprIdentifier* a) {
        SymbolVariable* sv = a->getSymbolVar();

        int local = cbuf->ldvar(sv);
        if (local != -1) {
            cbuf->_ldl(local);
        } else {
            throw "Unsupported variable. not local var.";
        }
    }

    static void visitExprLNumber(CodeBuf* cbuf, AstExprLNumber* a) {
        TokenType* typ = a->typ;

        if (typ == TK::L_I32) {
            cbuf->_ldc_i32(a->num.i32);
        } else {
            throw "Unknown literal number type";
        }
    }

    static void visitExprBinaryOp(CodeBuf* cbuf, AstExprBinaryOp* a) {
        TokenType* typ = a->typ;

        if (typ == TK::EQ) {
            visitExpression(cbuf, a->lhs);  // dstptr loaded.
            cbuf->_dup_ptr();
            mov(cbuf, a->rhs);  // load src, mov to dst.
        } else if (typ == TK::PLUS) {
            visitExpression(cbuf, a->lhs);
            visitExpression(cbuf, a->rhs);

            cbuf->_add_i32();
        } else {
            throw "Unknown binary op";
        }
    }



};

#endif //ETHERTIA_CODEGEN_H
