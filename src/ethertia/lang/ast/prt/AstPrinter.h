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

    static std::string printCompilationUnit(AstCompilationUnit* a) {
        AstPrinter::Prt prt;
        printCompilationUnit(prt, a);
        return prt.ss.str();
    }

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
        else if (CAST(AstStmtWhile*))    { printStmtWhile(s, c); }
        else if (CAST(AstStmtContinue*)) { printStmtContinue(s, c); }
        else if (CAST(AstStmtBreak*))    { printStmtBreak(s, c); }
        else if (CAST(AstStmtLabel*))    { printStmtLabel(s, c); }
        else if (CAST(AstStmtGoto*))     { printStmtGoto(s, c); }
        else if (CAST(AstStmtReturn*))   { printStmtReturn(s, c); }
        else if (CAST(AstStmtDefClass*)) { printStmtDefClass(s, c); }

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
            if (defv != a->params.front()) s.ad(", ");
            printStmtDefVar(s, defv, false);
        }
        s.ad(")");
        if (!a->body) s.ad(";");
        s.endline();

        if (a->body) {
            printStmtBlock(s, a->body);
        }
    }

    static void printModifiers(Prt& s, AstModifiers* a) {
        for (TokenType* tk : a->modifiers) {
            s.ad(tk->text);
            s.ad(" ");
        }
    }

    static void printStmtDefVar(Prt& s, AstStmtDefVar* a, bool semi = true) {
        if (semi)
            s.begline();
        printModifiers(s, a->mods);
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

    static void printStmtWhile(Prt& s, AstStmtWhile* a) {
        s.begline();
        s.ad("while (");
        printExpression(s, a->cond);
        s.ad(")");
        s.endline();

        printIndentStmt(s, a->body);

    }

    static void printStmtBlock(Prt& s, AstStmtBlock* a) {
        printStmtBlockStmts(s, a->stmts);
    }
    static void printStmtBlockStmts(Prt& s, std::vector<AstStmt*>& stmts) {
        s.line("{");
        s.pushIndent();
        printStmts(s, stmts);
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

    static void printStmtContinue(Prt& s, AstStmtContinue* a) {
        s.line("continue;");
    }

    static void printStmtBreak(Prt& s, AstStmtBreak* a) {
        s.line("break;");
    }

    static void printStmtLabel(Prt& s, AstStmtLabel* a) {
        s.line(a->name+":");
    }
    static void printStmtGoto(Prt& s, AstStmtGoto* a) {
        s.line("goto "+a->name+";");
    }

    static void printStmtReturn(Prt& s, AstStmtReturn* a) {
        s.begline();
        s.ad("return");
        if (a->ret) {
            s.ad(" ");
            printExpression(s, a->ret);
        }
        s.ad(";");
        s.endline();
    }

    static void printStmtDefClass(Prt& s, AstStmtDefClass* a) {
        s.begline();
        s.ad("class ");
        s.ad(a->name);
        s.endline();

        printStmtBlockStmts(s, a->stmts);
    }



    static void printExpression(Prt& s, AstExpr* a) {
        if (CAST(AstExprIdentifier*))        { s.ad(c->name); }
        else if (CAST(AstExprLNumber*))      { printExprLNumber(s, c); }
        else if (CAST(AstExprLString*))      { s.ad("\""); s.ad(c->str); s.ad("\""); }
        else if (CAST(AstExprMemberAccess*)) { printExprMemberAccess(s, c); }
        else if (CAST(AstExprBinaryOp*))     { printExprBinaryOp(s, c); }
        else if (CAST(AstExprUnaryOp*))      { printExprUnaryOp(s, c); }
        else if (CAST(AstExprFuncCall*))     { printExprFuncCall(s, c); }
        else if (CAST(AstExprSizeOf*))       { printExprSizeOf(s, c); }
        else if (CAST(AstExprTypeCast*))     { printExprTypeCast(s, c); }
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

    static void printExprUnaryOp(Prt& s, AstExprUnaryOp* a) {
        if (a->post) {
            printExpression(s, a->expr);
            s.ad(a->typ->text);
        } else {
            s.ad(a->typ->text);
            printExpression(s, a->expr);
        }
    }

    static void printExprLNumber(Prt& s, AstExprLNumber* a) {
        if (a->typ == TK::L_I32)      { s.ad(std::to_string(a->num.i32)); }
        else if (a->typ == TK::L_F32) { s.ad(std::to_string(a->num.f32)); }
        else if (a->typ == TK::TRUE)  { s.ad("true");  }
        else if (a->typ == TK::FALSE) { s.ad("false"); }
        else { throw "unsupported liternal number"; }
    }

    static void printExprFuncCall(Prt& s, AstExprFuncCall* a) {
        printExpression(s, a->expr);
        s.ad("(");
        for (int i = 0; i < a->args.size(); ++i) {
            if (i != 0) s.ad(", ");
            printExpression(s, a->args[i]);
        }
        s.ad(")");
    }

    static void printExprSizeOf(Prt& s, AstExprSizeOf* a) {
        s.ad("sizeof(");
        printExpression(s, a->expr);
        s.ad(")");
    }

    static void printExprTypeCast(Prt& s, AstExprTypeCast* a) {
        s.ad("(");
        printExpression(s, a->type);
        s.ad(")");
        printExpression(s, a->expr);
    }
};

#endif //ETHERTIA_ASTPRINTER_H
