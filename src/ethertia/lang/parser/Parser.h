//
// Created by Dreamtowards on 2022/5/4.
//

#ifndef ETHERTIA_PARSER_H
#define ETHERTIA_PARSER_H

#include <vector>

#include <ethertia/lang/lexer/Lexer.h>

#include <ethertia/lang/ast/AstStmt.h>
#include <ethertia/lang/ast/AstExpr.h>
#include <ethertia/lang/ast/AstMisc.h>

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

    // spec for Namespace
    static AstExpr* parsePathname(Lexer* lx) {
        AstExpr* lhs = parseExprIdentifier(lx);
        while (lx->nexting(TK::COLCOL)) {
            std::string rhs = parseIdentifier(lx);
            lhs = new AstExprMemberAccess(lhs, rhs, TK::COLCOL);
        }
        return lhs;
    }
    // redirect to parseExprSpec.
    static AstExpr* parseTypename(Lexer* lx) {
        return parsePathname(lx);
    }

    static std::string parseIdentifier(Lexer* lx) {
        lx->next(TK::L_IDENTIFIER);
        return lx->r_str;
    }
    static AstAttribute* parseAttribute(Lexer* lx) {
        lx->next(TK::AT);
        AstExpr* type = parseTypename(lx);

        std::vector<AstExpr*> args;
        if (lx->peeking(TK::LPAREN))
            args = parseExprFuncCallArgs(lx);

        return new AstAttribute(new AstExprFuncCall(type, args));
    }
    static std::vector<AstAttribute*> parseAttributes(Lexer* lx) {
        std::vector<AstAttribute*> attrs;
        while (lx->peeking(TK::AT)) {
            attrs.push_back(parseAttribute(lx));
        }
        return attrs;
    }

    static AstModifiers* parseModifiers(Lexer* lx) {
        std::vector<TokenType*> modifiers;
        TokenType* tk;
        while ((tk=lx->trynext(TokenType::MODIFIERS))) {
            modifiers.push_back(tk);
        }
        return new AstModifiers(modifiers);
    }


    static bool isPass(Lexer* lx, const std::function<void(Lexer*)>& psr) {
        try {
            psr(lx);
            return true;
        } catch (...) {
            return false;
        }
    }

    static AstStmt* parseTemplate(Lexer* lx) {
        lx->next(TK::LT);

        lx->next(TK::GT);
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

    static AstStmtReturn* parseStmtReturn(Lexer* lx) {
        lx->next(TK::RETURN);

        AstExpr* ret = nullptr;
        if (!lx->peeking(TK::SEMI)) {
            ret = parseExpression(lx);
        }
        lx->next(TK::SEMI);
        return new AstStmtReturn(ret);
    }

    static AstStmtBreak* parseStmtBreak(Lexer* lx) {
        lx->next(TK::BREAK);
        lx->next(TK::SEMI);
        return new AstStmtBreak();
    }
    static AstStmtContinue* parseStmtContinue(Lexer* lx) {
        lx->next(TK::CONTINUE);
        lx->next(TK::SEMI);
        return new AstStmtContinue();
    }

    static AstStmtBlank* parseStmtBlank(Lexer* lx) {
        lx->next(TK::SEMI);
        return new AstStmtBlank();
    }

    static std::vector<AstStmt*> parseStmtBlockStmts(Lexer* lx) {
        lx->next(TK::LBRACE);
        std::vector<AstStmt*> stmts;
        while (!lx->peeking(TK::RBRACE)) {
            stmts.push_back(parseStatement(lx));
        }
        lx->next(TK::RBRACE);
        return stmts;
    }
    static AstStmtBlock* parseStmtBlock(Lexer* lx) {
        return new AstStmtBlock(parseStmtBlockStmts(lx));
    }

    // using std::vector;                  // vector<int> sth;
    // using intlist = std::vector<int>;   // intlist sth;
    static AstStmt* parseStmtUsing(Lexer* lx) {
        lx->nexting(TK::USING);
        AstExpr* refer = parseTypename(lx);

        std::string alias;
        if (lx->nexting(TK::EQ)) {
            alias = ((AstExprIdentifier*)refer)->name;
            refer = parseTypename(lx);
        }
        lx->next(TK::SEMI);
        return new AstStmtUsing(refer, alias);
    }

    static AstStmtNamespace* parseStmtNamespace(Lexer* lx) {
        lx->next(TK::NAMESPACE);
        AstExpr* name = parsePathname(lx);

        std::vector<AstStmt*> stmts;
        if (lx->nexting(TK::SEMI)) {
            while (!lx->nexting(TK::NAMESPACE) && lx->clean_eof()) {
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


    static AstStmtDefClass* parseStmtDefClass(Lexer* lx) {
        lx->next(TK::CLASS);
        std::string name = parseIdentifier(lx);

        std::vector<AstExpr*> superclasses;
        if (lx->nexting(TK::COL)) {
            superclasses.push_back(parseTypename(lx));
            while (lx->nexting(TK::COMMA)) {
                superclasses.push_back(parseTypename(lx));
            }
        }

        std::vector<AstStmt*> stmts = parseStmtBlockStmts(lx);
        return new AstStmtDefClass(name, superclasses, stmts);
    }

    static AstStmtDefVar* parseStmtDefVar(Lexer* lx) {
        AstExpr* type = parseTypename(lx);
        std::string name = parseIdentifier(lx);

        AstExpr* init = nullptr;
        if (lx->nexting(TK::EQ)) {
            init = parseExpression(lx);
        }
        lx->next(TK::SEMI);
        return new AstStmtDefVar(type, name, init);
    }

    static AstStmtDefFunc* parseStmtDefFunc(Lexer* lx) {
        AstExpr* retType = parseTypename(lx);
        std::string name = parseIdentifier(lx);

        std::vector<AstStmtDefVar*> params;
        lx->next(TK::LPAREN);
        if (!lx->peeking(TK::RPAREN)) {
            params.push_back(parseStmtDefVar(lx));
            while (lx->peeking(TK::COMMA)) {
                params.push_back(parseStmtDefVar(lx));
            }
        }
        lx->next(TK::RPAREN);

        AstStmtBlock* body = parseStmtBlock(lx);

        return new AstStmtDefFunc(retType, name, params, body);
    }

    static AstStmtExpr* parseStmtExpr(Lexer* lx) {
        AstExpr* expr = parseExpression(lx);
        lx->next(TK::SEMI);
        return new AstStmtExpr(expr);
    }


    static AstStmt* parseStatement(Lexer* lx) {
        TokenType* tk = lx->peek();

        if (tk == TK::LBRACE) {
            return parseStmtBlock(lx);
        } else if (tk == TK::SEMI) {
            return parseStmtBlank(lx);
        } else if (tk == TK::IF) {
            return parseStmtIf(lx);
        } else if (tk == TK::WHILE) {
            return parseStmtWhile(lx);
        } else if (tk == TK::RETURN) {
            return parseStmtReturn(lx);
        } else if (tk == TK::BREAK) {
            return parseStmtBreak(lx);
        } else if (tk == TK::CONTINUE) {
            return parseStmtContinue(lx);
        } else if (tk == TK::USING) {
            return parseStmtUsing(lx);
        } else if (tk == TK::NAMESPACE) {
            return parseStmtNamespace(lx);
        } else {
            std::vector<AstAttribute*> attrs = parseAttributes(lx);
            AstModifiers* mods = parseModifiers(lx);

            if (tk == TK::CLASS) {
                auto* r = parseStmtDefClass(lx);
                return r;
            }

            int mark = lx->rdi;
            bool tan = isPass(lx, parseTypename) && isPass(lx, parseIdentifier);
            bool fn = lx->peeking(TK::LPAREN);
            lx->rdi = mark;  // setback.
            if (tan) {
                if (fn) {  // DefFunc.
                    return parseStmtDefFunc(lx);
                } else {   // DefVar
                    return parseStmtDefVar(lx);
                }
            }

            return parseStmtExpr(lx);
        }
    }










    static AstExprIdentifier* parseExprIdentifier(Lexer* lx) {
        lx->next(TK::L_IDENTIFIER);
        return new AstExprIdentifier(lx->r_str);
    }

    // 'ExprPrimary'. Literal, factors etc.
    static AstExpr* parseExprPrimary(Lexer* lx) {
        TokenType* tk = lx->peek();

        if (tk == TK::L_IDENTIFIER) {
            return parseExprIdentifier(lx);
        } else if (tk == TK::L_STRING) {
            lx->next(TK::L_STRING);
            return new AstExprLString(lx->r_str);
        } else {  // Numerical Literal
            lx->next();
            auto* n = new AstExprLNumber(tk);
                 if (tk == TK::L_CHAR) n->num.u32 = lx->r_integer;
            else if (tk == TK::TRUE)   n->num.i32 = 1;
            else if (tk == TK::FALSE)  n->num.i32 = 0;
            else if (tk == TK::L_I32)  n->num.i32 = lx->r_integer;
            else throw "Unknown literal number type";
            return n;
        }
        // sizeof, new, (..), *a, &a  ??
    }

    static std::vector<AstExpr*> parseExprFuncCallArgs(Lexer* lx) {
        lx->next(TK::LPAREN);
        std::vector<AstExpr*> args;
        if (!lx->peeking(TK::RPAREN)) {
            args.push_back(parseExpression(lx));
            while (lx->nexting(TK::COMMA)) {
                args.push_back(parseExpression(lx));
            }
        }
        lx->next(TK::RPAREN);
        return args;
    }

    // MemberAccess, FuncCall
    static AstExpr* parseExpr1_AccessCall(Lexer* lx) {
        AstExpr* lhs = parseExprPrimary(lx);
        while (true) {
            TokenType* tk = lx->peek();
            if (tk == TK::COLCOL || tk == TK::DOT || tk == TK::ARROW) {  // MemberAccess
                lx->next();
                std::string memb = parseIdentifier(lx);
                lhs = new AstExprMemberAccess(lhs, memb, tk);
            } else if (tk == TK::LPAREN) {  // FuncCall
                lhs = new AstExprFuncCall(lhs, parseExprFuncCallArgs(lx));
            } else {
                return lhs;  // pass.
            }
        }
    }

    // UnaryPost
    static AstExpr* parseExpr2_UnaryPost(Lexer* lx) {
        AstExpr* lhs = parseExpr1_AccessCall(lx);
        TokenType* tk;
        while ((tk=lx->peek()) == TK::PLUSPLUS || tk == TK::SUBSUB) {
            lx->next();
            lhs = new AstExprUnaryOp(lhs, true, tk);
        }
        return lhs;
    }

    // UnaryPre
    static AstExpr* parseExpr3_UnaryPre(Lexer* lx) {
        TokenType* tk = lx->peek();
        if (tk == TK::BANG || tk == TK::TILDE || tk == TK::STAR || tk == TK::AMP ||
            tk == TK::SUBSUB || tk == TK::PLUSPLUS || tk == TK::SUB) {
            lx->next();
            AstExpr* rhs = parseExpr3_UnaryPre(lx);
            return new AstExprUnaryOp(rhs, false, tk);
        } else if (tk == TK::LPAREN) {
            AstExpr* type = parseTypename(lx);
            lx->next(TK::RPAREN);
            AstExpr* rhs = parseExpr3_UnaryPre(lx);
            return new AstExprTypeCast(type, rhs);
        } else {
            return parseExpr2_UnaryPost(lx);
        }
    }

    static AstExpr* _parseBinOp_LR(Lexer* lx, const std::function<AstExpr*(Lexer*)>& psrfac, std::initializer_list<TokenType*> opers) {
        AstExpr* lhs = psrfac(lx);
        TokenType* tk;
        while ((tk=lx->trynext(opers))) {
            AstExpr* rhs = psrfac(lx);
            lhs = new AstExprBinaryOp(lhs, rhs, tk);
        }
        return lhs;
    }

    static AstExpr* parse4_BinMul(Lexer* lx) {
        return _parseBinOp_LR(lx, parseExpr3_UnaryPre, {TK::STAR, TK::SLASH, TK::PERCENT});
    }

    static AstExpr* parse5_BinAdd(Lexer* lx) {
        return _parseBinOp_LR(lx, parse4_BinMul, {TK::PLUS, TK::SUB});
    }

    static AstExpr* parse6_BinShift(Lexer* lx) {
        return _parseBinOp_LR(lx, parse5_BinAdd, {TK::LTLT, TK::GTGT});
    }

    static AstExpr* parse7_BinCmp(Lexer* lx) {
        return _parseBinOp_LR(lx, parse6_BinShift, {TK::LTEQ, TK::LT, TK::GTEQ, TK::GT});
    }

    static AstExpr* parse8_BinEq(Lexer* lx) {
        return _parseBinOp_LR(lx, parse7_BinCmp, {TK::EQEQ, TK::BANGEQ});
    }

    // Logic And/Or. should prior than bitwise and/or ops?
    static AstExpr* parse9_BinAndOr(Lexer* lx) {
        return _parseBinOp_LR(lx, parse8_BinEq, {TK::AMPAMP, TK::BARBAR});
    }

    static AstExpr* parse10_BinBitAndXorOr(Lexer* lx) {
        return _parseBinOp_LR(lx, parse9_BinAndOr, {TK::AMP, TK::CARET, TK::BAR});
    }

    // should be RL operator. but there simply no.
    static AstExpr* parse11_TriCond(Lexer* lx) {
        AstExpr* cond = parse10_BinBitAndXorOr(lx);
        if (lx->nexting(TK::QUES)) {
            AstExpr* then = parse10_BinBitAndXorOr(lx);
            lx->next(TK::COL);
            AstExpr* els = parse10_BinBitAndXorOr(lx);
            return new AstExprTriCond(cond, then, els);
        } else {
            return cond;
        }
    }

    static AstExpr* parse12_Assign(Lexer* lx) {
        AstExpr* lhs = parse11_TriCond(lx);
        if (lx->nexting(TK::EQ)) {
            AstExpr* rhs = parse12_Assign(lx);
            return new AstExprBinaryOp(lhs, rhs, TK::EQ);
        } else {
            return lhs;
        }
    }

    static AstExpr* parseExpression(Lexer* lx) {
        return parse12_Assign(lx);
    }
};

#endif //ETHERTIA_PARSER_H
