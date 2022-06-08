//
// Created by Dreamtowards on 2022/6/7.
//

#ifndef ETHERTIA_ASTPRINTER_H
#define ETHERTIA_ASTPRINTER_H

#include <ethertia/lang/ast/AstMisc.h>
#include <ethertia/lang/ast/AstStmt.h>
#include <ethertia/lang/ast/AstExpr.h>
#include <ethertia/util/Strings.h>


class AstPrinter
{
public:

    class Prt
    {
    public:
        std::stringstream ss;
        int indent = 0;

        void pushIndent() {
            indent += 4;
        }
        void popIndent() {
            indent -= 4;
        }
        void ad(const std::string& s) {
            ss << s;
        }
        void line(const std::string& s) {
            begline();
            ss << s;
            endline();
        }
        void begline() {
            for (int i = 0; i < indent; ++i) ss << ' ';
        }
        void endline() {
            ss << "\n";
        }
    };

    static void printCompilationUnit(Prt& s, AstCompilationUnit* a) {
        printStmts(s, a->stmts);
    }

    static void printStmts(Prt& s, std::vector<AstStmt*>& stmts) {
        for (AstStmt* stmt : stmts) {
            printStatement(s, stmt);
        }
    }

    static void printStatement(Prt& s, AstStmt* a) {
        if (CAST(AstStmtNamespace*))     { printStmtNamespace(s, c); }
        else if (CAST(AstStmtDefFunc*))  { printStmtDefFunc(s, c); }
        else if (CAST(AstStmtDefVar*))   { printStmtDefVar(s, c); }
        else if (CAST(AstStmtBlock*))    { printStmtBlock(s, c); }
        else if (CAST(AstStmtExpr*))     { printStmtExpr(s, c); }
        else if (CAST(AstStmtUsing*))    { printStmtUsing(s, c); }
        else if (CAST(AstStmtIf*))       { printStmtIf(s, c); }

        else { throw "Unsupported stmt"; }
    }

    static void printStmtNamespace(Prt& s, AstStmtNamespace* a) {
        s.line("namespace " + Strings::join("::", AstExprMemberAccess::namesExpand(a->name)));
        s.line("{");
        s.pushIndent();
        printStmts(s, a->stmts);
        s.popIndent();
        s.line("}");
    }

    static void printStmtDefFunc(Prt& s, AstStmtDefFunc* a) {
        s.begline();
        printExpression(s, a->retType);
        s.ad(" ");
        s.ad(a->name);
        s.ad("(");
        for (AstStmtDefVar* defv : a->params) {
            printStmtDefVar(s, defv, false);
        }
        s.ad(")");
        s.endline();

        printStmtBlock(s, a->body);
    }

    static void printStmtDefVar(Prt& s, AstStmtDefVar* a, bool semi = true) {
        if (semi)
            s.begline();
        printExpression(s, a->type);
        s.ad(" ");
        s.ad(a->name);

        if (a->init) {
            s.ad(" = ");
            printExpression(s, a->init);
        }
        if (semi)
            s.ad(";\n");
    }

    static void printStmtExpr(Prt& s, AstStmtExpr* a) {
        s.begline();
        printExpression(s, a->expr);
        s.ad(";\n");
    }

    static void printIndentStmt(Prt& s, AstStmt* a) {
        if (dynamic_cast<AstStmtBlock*>(a)) {
            printStatement(s, a);
        } else {
            s.pushIndent();
            printStatement(s, a);
            s.popIndent();
        }
    }

    static void printStmtIf(Prt& s, AstStmtIf* a) {
        s.begline();
        s.ad("if (");
        printExpression(s, a->cond);
        s.ad(")");
        s.endline();

        printIndentStmt(s, a->then);

        if (a->els) {
            s.line("else");
            printIndentStmt(s, a->els);
        }
    }

    static void printStmtBlock(Prt& s, AstStmtBlock* a) {
        s.line("{");
        s.pushIndent();
        printStmts(s, a->stmts);
        s.popIndent();
        s.line("}");
    }

    static void printStmtUsing(Prt& s, AstStmtUsing* a) {
        s.begline();
        s.ad("using ");
        s.ad(a->name);
        s.ad(" = ");
        printExpression(s, a->used);
        s.ad(";\n");
    }



    static void printExpression(Prt& s, AstExpr* a) {
        if (CAST(AstExprIdentifier*))        { s.ad(c->name); }
        else if (CAST(AstExprLNumber*))      { printExprLNumber(s, c); }
        else if (CAST(AstExprMemberAccess*)) { printExprMemberAccess(s, c); }
        else if (CAST(AstExprBinaryOp*))     { printExprBinaryOp(s, c); }
        else { throw "Unsupported expr"; }
    }

    static void printExprMemberAccess(Prt& s, AstExprMemberAccess* a) {
        printExpression(s, a->lhs);
        s.ad(a->typ->text);
        s.ad(a->memb);
    }

    static void printExprBinaryOp(Prt& s, AstExprBinaryOp* a) {
        printExpression(s, a->lhs);
        s.ad(" ");
        s.ad(a->typ->text);
        s.ad(" ");
        printExpression(s, a->rhs);
    }

    static void printExprLNumber(Prt& s, AstExprLNumber* a) {
        if (a->typ == TK::L_I32) {
            s.ad(std::to_string(a->num.i32));
        } else {
            throw "unsupported liternal number";
        }
    }
};

#endif //ETHERTIA_ASTPRINTER_H
