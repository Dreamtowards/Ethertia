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


    static void visitStmtDefFunc(CodeBuf* cbuf, AstStmtDefFunc* a) {

        for (AstStmtDefVar* param : a->params) {
            visitStmtDefVar(cbuf, param);
        }

        visitStmts(cbuf, a->body->stmts);

        cbuf->_nop();
    }

    static void visitStmtDefVar(CodeBuf* cbuf, AstStmtDefVar* a) {  cbuf->_verbo(a->str_v()+" :defvar_init");
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

    static void visitStmtExpr(CodeBuf* cbuf, AstStmtExpr* a) {  cbuf->_verbo(a->str_v()+" :stexpr");
        visitExpression(cbuf, a->expr);
        cbuf->_pop(a->expr->getSymbolVar()->getType()->getTypesize());
    }

    static void visitStmtIf(CodeBuf* cbuf, AstStmtIf* a) {  cbuf->_verbo(a->cond->str_v()+" :if_cond");
        // cond
        visitExpression(cbuf, a->cond);
        if (a->cond->getSymbolVar()->getType()->getTypesize() != 1)
            throw "Failed if condition. not a byte/boolean";
        u16 m_endthen = cbuf->_goto_f();  // if cond fail, goto endthen.
        u16 m_endelse;

        // then
        visitStatement(cbuf, a->then);

        if (a->els) {
            m_endelse = cbuf->_goto();
        }
        IO::st_16(cbuf->bufptr(m_endthen), cbuf->bufpos());  // endthen

        // else
        if (a->els) {
            visitStatement(cbuf, a->els);

            IO::st_16(cbuf->bufptr(m_endelse), cbuf->bufpos());  // endelse
        }
    }

    static void visitStmtWhile(CodeBuf* cbuf, AstStmtWhile* a) {  cbuf->_verbo(a->cond->str_v()+" :while_cond");
        // begloop
        u16 begwhile = cbuf->bufpos();

        t_ip old_loopbeg = cbuf->loop_beg;
        std::vector<t_ip> old_loopendms = cbuf->loop_end_mgoto;
        cbuf->loop_beg = begwhile;
        cbuf->loop_end_mgoto = {};

        // cond
        visitExpression(cbuf, a->cond);
        cbuf->loop_end_mgoto.push_back(cbuf->_goto_f());

        // body
        visitStatement(cbuf, a->body);
        cbuf->_goto(begwhile);

        t_ip ip_endwhile = cbuf->bufpos();
        for (t_ip end_mgoto : cbuf->loop_end_mgoto) {
            IO::st_16(cbuf->bufptr(end_mgoto), ip_endwhile);
        }

        cbuf->loop_beg = old_loopbeg;
        cbuf->loop_end_mgoto = old_loopendms;
    }

    static void visitStmtContinue(CodeBuf* cbuf, AstStmtContinue* a) {  cbuf->_verbo(a->str_v());
        // check have enclosing loop.
        cbuf->_goto(cbuf->loop_beg);
    }

    static void visitStmtBreak(CodeBuf* cbuf, AstStmtBreak* a) {  cbuf->_verbo(a->str_v());
        cbuf->loop_end_mgoto.push_back(cbuf->_goto());
    }

    static void visitStmts(CodeBuf* cbuf, const std::vector<AstStmt*>& stmts) {
        for (AstStmt* stmt : stmts) {
            visitStatement(cbuf, stmt);
        }
    }

    static void visitStatement(CodeBuf* cbuf, AstStmt* a) {
             if (CAST(AstStmtBlock*))     { visitStmts(cbuf, c->stmts);  }
        else if (CAST(AstStmtIf*))        { visitStmtIf(cbuf, c);        }
        else if (CAST(AstStmtWhile*))     { visitStmtWhile(cbuf, c);     }
        else if (CAST(AstStmtContinue*))  { visitStmtContinue(cbuf, c); }
        else if (CAST(AstStmtBreak*))     { visitStmtBreak(cbuf, c); }
        else if (CAST(AstStmtUsing*))     { }
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

    static void makesure_rvalue(CodeBuf* cbuf, AstExpr* a) {

        SymbolVariable* sv = a->getSymbolVar();
        if (sv->lvalue()) {

            cbuf->_mov_push(sv->getType()->getTypesize());
        }
    }

    static void visitExprBinaryOp(CodeBuf* cbuf, AstExprBinaryOp* a) {
        TokenType* typ = a->typ;

        if (typ == TK::EQ) {
            visitExpression(cbuf, a->lhs);  // dstptr loaded.
            cbuf->_dup_ptr();
            mov(cbuf, a->rhs);  // load src, mov to dst.
        } else {
            visitExpression(cbuf, a->lhs);  makesure_rvalue(cbuf, a->lhs);
            visitExpression(cbuf, a->rhs);  makesure_rvalue(cbuf, a->rhs);

            if (typ == TK::PLUS) {
                cbuf->_add_i32();
            } else if (typ == TK::GT) {
                cbuf->_icmp(Opcodes::ICMP_SGT, Opcodes::ICMP_I32);
            } else if (typ == TK::LT) {
                cbuf->_icmp(Opcodes::ICMP_SLT, Opcodes::ICMP_I32);
            } else {
                throw "Unknown binary op";
            }
        }
    }



};

#endif //ETHERTIA_CODEGEN_H
