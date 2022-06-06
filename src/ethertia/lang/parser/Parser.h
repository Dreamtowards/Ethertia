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

#define AEND(a) initlc(a, beg, lx)
#define ABEG int beg = lx->rdi_clean()

    template<typename T>
    static T* initlc(T* a, int beg, Lexer* lx) {
        a->initlc(beg, lx);
        return a;
    }

    static AstCompilationUnit* parseCompilationUnit(Lexer* lx) {  ABEG;
        std::vector<AstStmt*> stmts;
        while (!lx->clean_eof()) {
            stmts.push_back(parseStatement(lx));
        }
        return AEND(new AstCompilationUnit(stmts));
    }

    // spec for Namespace
    static AstExpr* parsePathname(Lexer* lx) {  ABEG;
        AstExpr* lhs = parseExprIdentifier(lx);
        while (lx->nexting(TK::COLCOL)) {
            std::string rhs = parseIdentifier(lx);
            lhs = AEND(new AstExprMemberAccess(lhs, rhs, TK::COLCOL));
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
    static AstAttribute* parseAttribute(Lexer* lx) {  ABEG;
        lx->next(TK::AT);
        AstExpr* type = parseTypename(lx);

        // maybe there only need a parseExprAccessCall, no matter what syntax, just check on Symbolization.
        std::vector<AstExpr*> args;  int beg_f = lx->rdi_clean();
        if (lx->peeking(TK::LPAREN))
            args = parseExprFuncCallArgs(lx);
        AstExprFuncCall* init = initlc(new AstExprFuncCall(type, args), beg_f, lx);

        return AEND(new AstAttribute(init));
    }
    static std::vector<AstAttribute*> parseAttributes(Lexer* lx) {
        std::vector<AstAttribute*> attrs;
        while (lx->peeking(TK::AT)) {
            attrs.push_back(parseAttribute(lx));
        }
        return attrs;
    }

    static AstModifiers* parseModifiers(Lexer* lx) {  ABEG;
        std::vector<TokenType*> modifiers;
        TokenType* tk;
        while ((tk=lx->trynext(TokenType::MODIFIERS))) {
            modifiers.push_back(tk);
        }
        return AEND(new AstModifiers(modifiers));
    }


    static bool isPass(Lexer* lx, const std::function<void(Lexer*)>& psr) {
        try {
            psr(lx);
            return true;
        } catch (...) {
            return false;
        }
    }

    static AstTemplate* parseTemplate(Lexer* lx) {  ABEG;
        lx->next(TK::LT);

        std::vector<std::pair<std::string, std::string>> temps;
        do {
            std::string typname = "typename";
            std::string name = parseIdentifier(lx);

            if (lx->peeking(TK::L_IDENTIFIER)) {
                typname = name;
                name = parseIdentifier(lx);
            }
            temps.emplace_back(typname, name);
        } while (lx->nexting(TK::COMMA));

        lx->next(TK::GT);
        return AEND(new AstTemplate(temps));
    }

    // Proc-Control Statements.

    static AstStmtIf* parseStmtIf(Lexer* lx) {  ABEG;
        lx->next(TK::IF);
        lx->next(TK::LPAREN);
        AstExpr* cond = parseExpression(lx);
        lx->next(TK::RPAREN);
        AstStmt* then = parseStatement(lx);

        AstStmt* els = nullptr;
        if (lx->nexting(TK::ELSE)) {
            els = parseStatement(lx);
        }
        return AEND(new AstStmtIf(cond, then, els));
    }

    static AstStmtWhile* parseStmtWhile(Lexer* lx) {  ABEG;
        lx->next(TK::WHILE);
        lx->next(TK::LPAREN);
        AstExpr* cond = parseExpression(lx);
        lx->next(TK::RPAREN);

        AstStmt* body = parseStatement(lx);
        return AEND(new AstStmtWhile(cond, body));
    }

    static AstStmtReturn* parseStmtReturn(Lexer* lx) {  ABEG;
        lx->next(TK::RETURN);

        AstExpr* ret = nullptr;
        if (!lx->peeking(TK::SEMI)) {
            ret = parseExpression(lx);
        }
        lx->next(TK::SEMI);
        return AEND(new AstStmtReturn(ret));
    }

    static AstStmtBreak* parseStmtBreak(Lexer* lx) {  ABEG;
        lx->next(TK::BREAK);
        lx->next(TK::SEMI);
        return AEND(new AstStmtBreak());
    }
    static AstStmtContinue* parseStmtContinue(Lexer* lx) {  ABEG;
        lx->next(TK::CONTINUE);
        lx->next(TK::SEMI);
        return AEND(new AstStmtContinue());
    }

    static AstStmtBlank* parseStmtBlank(Lexer* lx) {  ABEG;
        lx->next(TK::SEMI);
        return AEND(new AstStmtBlank());
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
    static AstStmtBlock* parseStmtBlock(Lexer* lx) {  ABEG;
        return AEND(new AstStmtBlock(parseStmtBlockStmts(lx)));
    }

    // using std::vector;                  // vector<int> sth;
    // using intlist = std::vector<int>;   // intlist sth;
    static AstStmt* parseStmtUsing(Lexer* lx) {  ABEG;
        lx->nexting(TK::USING);
        AstExpr* used = parseTypename(lx);

        std::string name = *(AstExprMemberAccess::namesExpand(used).end());
        if (lx->nexting(TK::EQ)) {
            name = ((AstExprIdentifier*)used)->name;
            used = parseTypename(lx);
        }
        lx->next(TK::SEMI);
        return AEND(new AstStmtUsing(used, name));
    }

    static AstStmtNamespace* parseStmtNamespace(Lexer* lx) {  ABEG;
        lx->next(TK::NAMESPACE);
        AstExpr* name = parsePathname(lx);

        std::vector<AstStmt*> stmts;
        if (lx->nexting(TK::SEMI)) {
            while (!lx->nexting(TK::NAMESPACE) && !lx->clean_eof()) {
                stmts.push_back(parseStatement(lx));
            }
        } else {
            lx->next(TK::LBRACE);
            while (!lx->nexting(TK::RBRACE)) {
                stmts.push_back(parseStatement(lx));
            }
        }
        return AEND(new AstStmtNamespace(name, stmts));
    }


    static AstStmtDefClass* parseStmtDefClass(Lexer* lx) {  ABEG;
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
        return AEND(new AstStmtDefClass(name, superclasses, stmts));
    }

    static AstStmtDefVar* parseStmtDefVar(Lexer* lx) {  ABEG;
        AstExpr* type = parseTypename(lx);
        std::string name = parseIdentifier(lx);

        AstExpr* init = nullptr;
        if (lx->nexting(TK::EQ)) {
            init = parseExpression(lx);
        }
        lx->next(TK::SEMI);
        return AEND(new AstStmtDefVar(type, name, init));
    }

    static AstStmtDefFunc* parseStmtDefFunc(Lexer* lx) {  ABEG;
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

        return AEND(new AstStmtDefFunc(retType, name, params, body));
    }

    static AstStmtExpr* parseStmtExpr(Lexer* lx) {  ABEG;
        AstExpr* expr = parseExpression(lx);
        lx->next(TK::SEMI);
        return AEND(new AstStmtExpr(expr));
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
            parseAttributes(lx);
            parseModifiers(lx);

            if (tk == TK::CLASS) {
                auto* r = parseStmtDefClass(lx);
                return r;
            }

            int mark = lx->rdi;
            bool prelead = isPass(lx, parseTypename) && isPass(lx, parseIdentifier);
            bool fn = lx->peeking(TK::LPAREN) || lx->peeking(TK::LT);
            lx->rdi = mark;  // setback.
            if (prelead) {
                if (fn) {  // DefFunc.
                    return parseStmtDefFunc(lx);
                } else {   // DefVar
                    return parseStmtDefVar(lx);
                }
            }

            return parseStmtExpr(lx);
        }
    }










    static AstExprIdentifier* parseExprIdentifier(Lexer* lx) {  ABEG;
        lx->next(TK::L_IDENTIFIER);
        return AEND(new AstExprIdentifier(lx->r_str));
    }

    // 'ExprPrimary'. Literal, factors etc.
    static AstExpr* parseExprPrimary(Lexer* lx) {  ABEG;
        TokenType* tk = lx->peek();

        // new, sizeof
        if (tk == TK::L_IDENTIFIER) {
            return parseExprIdentifier(lx);
        } else if (tk == TK::L_STRING) {
            lx->next(TK::L_STRING);
            return AEND(new AstExprLString(lx->r_str));
        } else {  // Numerical Literal
            lx->next();
            auto* n = AEND(new AstExprLNumber(tk));
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
    static AstExpr* parseExpr1_AccessCall(Lexer* lx) {  ABEG;
        AstExpr* lhs = parseExprPrimary(lx);
        while (true) {
            TokenType* tk = lx->peek();
            if (tk == TK::COLCOL || tk == TK::DOT || tk == TK::ARROW) {  // MemberAccess
                lx->next();
                std::string memb = parseIdentifier(lx);
                lhs = AEND(new AstExprMemberAccess(lhs, memb, tk));
            } else if (tk == TK::LPAREN) {  // FuncCall
                lhs = AEND(new AstExprFuncCall(lhs, parseExprFuncCallArgs(lx)));
            } else {
                return lhs;  // pass.
            }
        }
    }

    // UnaryPost
    static AstExpr* parseExpr2_UnaryPost(Lexer* lx) {  ABEG;
        AstExpr* lhs = parseExpr1_AccessCall(lx);
        TokenType* tk;
        while ((tk=lx->peek()) == TK::PLUSPLUS || tk == TK::SUBSUB) {
            lx->next();
            lhs = AEND(new AstExprUnaryOp(lhs, true, tk));
        }
        return lhs;
    }

    // UnaryPre
    static AstExpr* parseExpr3_UnaryPre(Lexer* lx) {  ABEG;
        TokenType* tk = lx->peek();
        if (tk == TK::BANG || tk == TK::TILDE || tk == TK::STAR || tk == TK::AMP ||
            tk == TK::SUBSUB || tk == TK::PLUSPLUS || tk == TK::SUB) {
            lx->next();
            AstExpr* rhs = parseExpr3_UnaryPre(lx);
            return AEND(new AstExprUnaryOp(rhs, false, tk));
        } else if (tk == TK::LPAREN) {
            AstExpr* type = parseTypename(lx);
            lx->next(TK::RPAREN);
            AstExpr* rhs = parseExpr3_UnaryPre(lx);
            return AEND(new AstExprTypeCast(type, rhs));
        } else {
            return parseExpr2_UnaryPost(lx);
        }
    }

    static AstExpr* _parseBinOp_LR(Lexer* lx, const std::function<AstExpr*(Lexer*)>& psrfac, std::initializer_list<TokenType*> opers) {  ABEG;
        AstExpr* lhs = psrfac(lx);
        TokenType* tk;
        while ((tk=lx->trynext(opers))) {
            AstExpr* rhs = psrfac(lx);
            lhs = AEND(new AstExprBinaryOp(lhs, rhs, tk));
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
    static AstExpr* parse11_TriCond(Lexer* lx) {  ABEG;
        AstExpr* cond = parse10_BinBitAndXorOr(lx);
        if (lx->nexting(TK::QUES)) {
            AstExpr* then = parse10_BinBitAndXorOr(lx);
            lx->next(TK::COL);
            AstExpr* els = parse10_BinBitAndXorOr(lx);
            return AEND(new AstExprTriCond(cond, then, els));
        } else {
            return cond;
        }
    }

    static AstExpr* parse12_Assign(Lexer* lx) {  ABEG;
        AstExpr* lhs = parse11_TriCond(lx);
        if (lx->nexting(TK::EQ)) {
            AstExpr* rhs = parse12_Assign(lx);
            return AEND(new AstExprBinaryOp(lhs, rhs, TK::EQ));
        } else {
            return lhs;
        }
    }

    static AstExpr* parseExpression(Lexer* lx) {
        return parse12_Assign(lx);
    }
};

#endif //ETHERTIA_PARSER_H
