//
// Created by Dreamtowards on 2022/5/3.
//

#ifndef ETHERTIA_TOKENTYPE_H
#define ETHERTIA_TOKENTYPE_H

#include <vector>

class TokenType
{
public:
    const char* text = nullptr;

    TokenType() { ALL.push_back(this); }
    TokenType(const char* text) : text(text) { ALL.push_back(this); }

    static std::vector<TokenType*> ALL;

    static TokenType L_IDENTIFIER;
    static TokenType L_U32;
    static TokenType L_U64;
    static TokenType L_I32;
    static TokenType L_I64;
    static TokenType L_F32;
    static TokenType L_F64;
    static TokenType L_CHAR;
    static TokenType L_STRING;


    static constexpr TokenType* DEF_I = &L_I32;
    static constexpr TokenType* DEF_F = &L_F32;

    static bool isFp(TokenType* tk) {
        return tk == &L_F32 || tk == &L_F64;
    }
    static bool isNumber(TokenType* tk) {
        return isFp(tk) ||
               tk == &L_U32 || tk == &L_U64 ||
               tk == &L_I32 || tk == &L_I64;
    }

    static TokenType SIZEOF;
    static TokenType NAMESPACE;
    static TokenType USING;
    static TokenType NEW;
    static TokenType IS;
    static TokenType AS;
    static TokenType CLASS;
    static TokenType VAR;

    static TokenType STATIC;
    static TokenType CONST;

    static TokenType IF;
    static TokenType ELSE;
    static TokenType WHILE;
    static TokenType CONTINUE;
    static TokenType BREAK;
    static TokenType RETURN;
    static TokenType FOR;
    static TokenType GOTO;
    // case catch default do enum goto switch throw try void
    // private protected public super transient volatile

    static TokenType TRUE;
    static TokenType FALSE;
    static TokenType NULLPTR;
    static TokenType THIS;


    static TokenType AMPAMP;
    static TokenType AMP;
    static TokenType BARBAR;
    static TokenType BAR;
    static TokenType COLCOL;
    static TokenType COL;
    static TokenType DOTDOTDOT;
    static TokenType DOT;
    static TokenType PLUSEQ;
    static TokenType PLUSPLUS;
    static TokenType PLUS;
    static TokenType ARROW;
    static TokenType SUBEQ;
    static TokenType SUBSUB;
    static TokenType SUB;

    static TokenType EQEQ;
    static TokenType EQ;
    static TokenType BANGEQ;
    static TokenType BANG;
    static TokenType GTEQ;
    static TokenType GTGT;
    static TokenType GT;
    static TokenType LTEQ;
    static TokenType LTLT;
    static TokenType LT;


    static TokenType QUES;
    static TokenType CARET;
    static TokenType TILDE;
    static TokenType AT;
    static TokenType PERCENT;
    static TokenType COMMA;
    static TokenType STAR;
    static TokenType SLASH;
    static TokenType SEMI;

    static TokenType LPAREN;
    static TokenType RPAREN;
    static TokenType LBRACE;
    static TokenType RBRACE;
    static TokenType LBRACKET;
    static TokenType RBRACKET;


    inline static std::vector<TokenType*> MODIFIERS = {
            &CONST,
            &STATIC  // &GET, &SET
    };

};

#endif //ETHERTIA_TOKENTYPE_H
