//
// Created by Dreamtowards on 2022/5/4.
//

#ifndef ETHERTIA_PARSER_H
#define ETHERTIA_PARSER_H

#include <vector>

#include <ethertia/lang/lexer/Lexer.h>

#include <ethertia/lang/ast/AstStmt.h>
#include <ethertia/lang/ast/AstExpr.h>

#define TK &TokenType

class Parser
{
public:

    static AstCompilationUnit parseCompilationUnit(Lexer* lx) {
        std::vector<AstStmt*> stmts;
        while (!lx->eof()) {
            stmts.push_back(parseStatement(lx));
        }
        return AstCompilationUnit(stmts);
    }

    // Proc-Control Statements.

    static AstStmtIf* parseStmtIf(Lexer* lx) {
        lx->next(TK::IF);
        lx->next(TK::LPAREN);
        AstExpr* cond = parseExpression(lx);
        lx->next(TK::RPAREN);

        AstStmt* then = parseStatement(lx);

        AstStmt* els = nullptr;
        if (lx->nexting(TK::ELSE)) {
            els = parseStatement(lx);
        }
        return new AstStmtIf(cond, then, els);
    }

    static AstStmtWhile* parseStmtWhile(Lexer* lx) {
        lx->next(TK::WHILE);
        lx->next(TK::LPAREN);
        AstExpr* cond = parseExpression(lx);
        lx->next(TK::RPAREN);

        AstStmt* body = parseStatement(lx);
        return new AstStmtWhile(cond, body);
    }

    static AstExpr* tmpParsePathName(Lexer* lx) {
        AstExpr* lhs = parseExpression(lx);
        while (lx->nexting(TK::COLCOL)) {
            AstExpr* rhs = parseExprIdentifier(lx);
            lhs = new AstExprMemberAccess(lhs, rhs, AstExprMemberAccess::T_PATH);
        }
        return lhs;
    }

    static AstStmt* parseStmtUsing(Lexer* lx) {
        lx->nexting(TK::USING);
        AstExpr* refer = tmpParsePathName(lx);

        std::string alias;
        if (lx->nexting(TK::EQ)) {
            alias = ((AstExprIdentifier*)refer)->name;
            refer = tmpParsePathName(lx);
        }
        lx->next(TK::SEMI);
        return new AstStmtUsing(refer, alias);
    }

    static AstStmtNamespace* parseStmtNamespace(Lexer* lx) {
        lx->next(TK::NAMESPACE);
        AstExpr* name = tmpParsePathName(lx);

        std::vector<AstStmt*> stmts;
        if (lx->nexting(TK::SEMI)) {
            while (!lx->nexting(TK::NAMESPACE)) {
                stmts.push_back(parseStatement(lx));
            }
        } else {
            lx->next(TK::LBRACE);
            while (!lx->nexting(TK::RBRACE)) {
                stmts.push_back(parseStatement(lx));
            }
        }
        return new AstStmtNamespace(name, stmts);
    }

    static AstStmt* parseStatement(Lexer* lx) {
        TokenType* tk = lx->peek();

        if (tk == TK::IF) {
            return parseStmtIf(lx);
        } else if (tk == TK::WHILE) {
            return parseStmtWhile(lx);
        } else if (tk == TK::USING) {
            return parseStmtUsing(lx);
        } else if (tk == TK::NAMESPACE) {
            return parseStmtNamespace(lx);
//        } else if (tk == TK::CLASS) {
        } else {
            throw "Unsupported";
        }
    }



    static AstExprIdentifier* parseExprIdentifier(Lexer* lx) {
        lx->next(TK::L_IDENTIFIER);
        return new AstExprIdentifier(lx->r_str);
    }

    static AstExpr* parseExpression(Lexer* lx) {

    }
};

#endif //ETHERTIA_PARSER_H
