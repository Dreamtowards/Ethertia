//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_CODEGEN_H
#define ETHERTIA_CODEGEN_H

#include <ethertia/lang/compiler/CodeBuf.h>
#include <ethertia/lang/ast/AstMisc.h>
#include <ethertia/lang/symbol/Scope.h>

#include <set>

class CodeGen
{
public:


    static void visitStatement(CodeBuf* cbuf, AstStmt* a) {
        if (CAST(AstStmtBlock*))          { visitStmtBlock(cbuf, c);  }
        else if (CAST(AstStmtIf*))        { visitStmtIf(cbuf, c);        }
        else if (CAST(AstStmtWhile*))     { visitStmtWhile(cbuf, c);     }
        else if (CAST(AstStmtContinue*))  { visitStmtContinue(cbuf, c); }
        else if (CAST(AstStmtBreak*))     { visitStmtBreak(cbuf, c); }
        else if (CAST(AstStmtLabel*))     { visitStmtLabel(cbuf, c); }
        else if (CAST(AstStmtGoto*))      { visitStmtGoto(cbuf, c); }
        else if (CAST(AstStmtUsing*))     { }
        else if (CAST(AstStmtReturn*))    { visitStmtReturn(cbuf, c);    }
        else if (CAST(AstStmtDefVar*))    { visitStmtDefVar(cbuf, c);    }
        else if (CAST(AstStmtExpr*))      { visitStmtExpr(cbuf, c);      }
        else { throw "Unsupported statement."; }
    }

    static void visitStmts(CodeBuf* cbuf, const std::vector<AstStmt*>& stmts) {
        for (AstStmt* stmt : stmts) {
            visitStatement(cbuf, stmt);
        }
    }


    inline static std::set<std::string> traceUniqueLocalVars;
    inline static int localvarp = 0;
    inline static bool isVisitParameters = false;

    static void visitStmtBlock(CodeBuf* cbuf, AstStmtBlock* a) {
        int old_localvp = localvarp;

        visitStmts(cbuf, a->stmts);

        u32 blocksize = localvarp - old_localvp;
        if (blocksize != 0)
            cbuf->_pop(blocksize);

        localvarp = old_localvp;
    }

    static void mov(CodeBuf* cbuf, AstExpr* expr) {
        visitExpression(cbuf, expr);  // src ptr.

        SymbolVariable* sv = expr->getSymbolVar();
        u8 tsize = sv->getType()->getTypesize();
        if (sv->rvalue()) {
            cbuf->_pop_mov(tsize);
        } else {
            cbuf->_mov(tsize);
        }
    }

    static void visitStmtDefFunc(CodeBuf* cbuf, AstStmtDefFunc* a) {

        localvarp = 0;
        traceUniqueLocalVars.clear();
        isVisitParameters = true;
        for (AstStmtDefVar* param : a->params) {
            visitStmtDefVar(cbuf, param);
        }
        isVisitParameters = false;

        visitStmtBlock(cbuf, a->body);

        cbuf->_ret();

        // complete stmt_goto dst_ips.
        for (auto& mgoto : cbuf->labels_mgotos) {
            auto it = cbuf->labels.find(mgoto.second);
            if (it == cbuf->labels.end()) throw "goto unknown label";
            t_ip dst_ip = it->second;
            IO::st_16(cbuf->bufptr(mgoto.first), dst_ip);
        }
    }

    static void visitStmtDefVar(CodeBuf* cbuf, AstStmtDefVar* a) {  cbuf->_verbo(a->str_v()+" :defvar_init");
        u32 tsize = a->vsymbol->getType()->getTypesize();
        a->vsymbol->var_lpos = localvarp;
        localvarp += tsize;

        if (traceUniqueLocalVars.find(a->name) != traceUniqueLocalVars.end())
            throw "Local var name already defined";
        traceUniqueLocalVars.insert(a->name);

        if (!isVisitParameters)
            cbuf->_push(tsize);

        if (a->init) {
            AstExpr* expr = a->init;
            // assign
            cbuf->_ldl(a->vsymbol);
            mov(cbuf, expr);
        }
    }

    static void visitStmtExpr(CodeBuf* cbuf, AstStmtExpr* a) {  cbuf->_verbo(a->str_v()+" :stexpr");

        visitExpression(cbuf, a->expr);

        SymbolVariable* sv = a->expr->getSymbolVar();
        if (sv->lvalue()) {
            cbuf->_pop(SymbolIntlPointer::PTR_SIZE);
        } else {
            cbuf->_pop(sv->getType()->getTypesize());
        }
    }

    static void visitStmtIf(CodeBuf* cbuf, AstStmtIf* a) {  cbuf->_verbo(a->cond->str_v()+" :if_cond");
        // cond
        visitExpression(cbuf, a->cond);  makesure_rvalue(cbuf, a->cond);
        if (a->cond->getSymbolVar()->getType()->getTypesize() != 1)
            throw "Failed if condition. not a byte/boolean";
        u16 m_endthen = cbuf->_jmp_f();  // if cond fail, goto endthen.
        u16 m_endelse;

        // then
        visitStatement(cbuf, a->then);

        if (a->els) {
            m_endelse = cbuf->_jmp();
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
        visitExpression(cbuf, a->cond);  makesure_rvalue(cbuf, a->cond);
        cbuf->loop_end_mgoto.push_back(cbuf->_jmp_f());

        // body
        visitStatement(cbuf, a->body);
        cbuf->_jmp(begwhile);

        t_ip ip_endwhile = cbuf->bufpos();
        for (t_ip end_mgoto : cbuf->loop_end_mgoto) {
            IO::st_16(cbuf->bufptr(end_mgoto), ip_endwhile);
        }

        cbuf->loop_beg = old_loopbeg;
        cbuf->loop_end_mgoto = old_loopendms;
    }

    static void visitStmtContinue(CodeBuf* cbuf, AstStmtContinue* a) {  cbuf->_verbo(a->str_v());
        // check have enclosing loop.
        cbuf->_jmp(cbuf->loop_beg);
    }

    static void visitStmtBreak(CodeBuf* cbuf, AstStmtBreak* a) {  cbuf->_verbo(a->str_v());
        cbuf->loop_end_mgoto.push_back(cbuf->_jmp());
    }

    static void visitStmtLabel(CodeBuf* cbuf, AstStmtLabel* a) {
        if (cbuf->labels.find(a->name) != cbuf->labels.end())  // check on CodeGen instead of SymPhase. cuz labels have no their symtab.
            throw "label name had already defined.";

        cbuf->labels[a->name] = cbuf->bufpos();
    }

    static void visitStmtGoto(CodeBuf* cbuf, AstStmtGoto* a) {
        // later fill.
        cbuf->labels_mgotos.emplace_back(
            cbuf->_jmp(),
            a->name
        );
    }

    static void visitStmtReturn(CodeBuf* cbuf, AstStmtReturn* a) {  cbuf->_verbo(a->str_v());

        if (a->ret) {

            visitExpression(cbuf, a->ret);  makesure_rvalue(cbuf, a->ret);

            cbuf->_ldl((u16)0);  // dst-ptr: func-stack-begin
            cbuf->_ldl(localvarp);  // src-ptr: stmt-begin.

            cbuf->_mov(a->ret->getSymbolVar()->getType()->getTypesize());
        }

        // pop
        cbuf->_pop(localvarp);

        cbuf->_ret();
    }






    static void visitExpression(CodeBuf* cbuf, AstExpr* a) {
             if (CAST(AstExprIdentifier*))   { visitExprIdentifier(cbuf, c); }
        else if (CAST(AstExprLNumber*))      { visitExprLNumber(cbuf, c);    }
        else if (CAST(AstExprLString*))      { throw "unsupp"; }
//        else if (CAST(AstExprMemberAccess*)) { visitExprMemberAccess(cbuf, c);}
        else if (CAST(AstExprFuncCall*))     { visitExprFuncCall(cbuf, c); }
        else if (CAST(AstExprUnaryOp*))      { visitExprUnaryOp(cbuf, c); }
        else if (CAST(AstExprBinaryOp*))     { visitExprBinaryOp(cbuf, c); }
        else if (CAST(AstExprTypeCast*))     { visitExpression(cbuf, c->expr);  }
        else if (CAST(AstExprSizeOf*))       { visitExprSizeOf(cbuf, c); }
        else if (CAST(AstExprTriCond*))      { /* visitExprTriCond(s, c);*/ }
        else { throw "Unsupported expression."; }
    }

    static void visitExprIdentifier(CodeBuf* cbuf, AstExprIdentifier* a) {
        SymbolVariable* sv = a->getSymbolVar();

        if (Modifiers::isStatic(sv->mods)) {
            cbuf->_lds(sv->var_spos);
        } else {
            cbuf->_ldl(sv);
        }
    }

    static void visitExprLNumber(CodeBuf* cbuf, AstExprLNumber* a) {
        TokenType* typ = a->typ;

        if (typ == TK::L_I32)       { cbuf->_ldc_i32(a->num.i32); }
        else if (typ == TK::TRUE)   { cbuf->_ldc_i8(1); }
        else if (typ == TK::FALSE)  { cbuf->_ldc_i8(0); }
        else { throw "Unknown literal number type"; }
    }

    static void makesure_rvalue(CodeBuf* cbuf, AstExpr* a) {

        SymbolVariable* sv = a->getSymbolVar();
        if (sv->lvalue()) {

            cbuf->_ldv(sv->getType()->getTypesize());
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

    static void visitExprUnaryOp(CodeBuf* cbuf, AstExprUnaryOp* a) {
        TokenType* typ = a->typ;

        if (a->post) {
            throw "Unsupp";
        } else {
            if (typ == TK::AMP) {
                visitExpression(cbuf, a->expr);
            } else if (typ == TK::STAR) {
                visitExpression(cbuf, a->expr);  makesure_rvalue(cbuf, a->expr);
            } else {
                throw "unsupp";
            }
        }
    }

    static void visitExprFuncCall(CodeBuf* cbuf, AstExprFuncCall* a) {

        // makesure desurgared
        u16 args_bytes = 0;
        for (AstExpr* arg : a->args) {
            args_bytes += arg->getSymbolVar()->getType()->getTypesize();
            // put arg.
            visitExpression(cbuf, arg);
        }

        SymbolFunction* sf = (SymbolFunction*)a->expr->getSymbol();

        if (Modifiers::isNative(sf->mods)) {
            std::string fname = sf->getQualifiedName();

            cbuf->_calli(args_bytes, fname);
        } else {
            int stloc_fn = sf->code_spos;
            if (stloc_fn == -1)
                throw "Undefined func";
            cbuf->_call(args_bytes, stloc_fn);
        }

    }

    static void visitExprSizeOf(CodeBuf* cbuf, AstExprSizeOf* a) {
        TypeSymbol* st = a->expr->getSymbolType();

        int size = st->getTypesize();
        cbuf->_ldc_i32(size);  // u32
    }


};

#endif //ETHERTIA_CODEGEN_H
