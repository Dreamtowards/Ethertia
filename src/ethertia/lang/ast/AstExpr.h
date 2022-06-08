//
// Created by Dreamtowards on 2022/5/4.
//

#ifndef ETHERTIA_ASTEXPR_H
#define ETHERTIA_ASTEXPR_H

#include <utility>

#include "Ast.h"
#include <ethertia/lang/symbol/Symbol.h>
#include <ethertia/lang/symbol/SymbolVariable.h>

class AstExpr : public Ast
{
    Symbol* symbol = nullptr;

public:

    void setSymbol(Symbol* sym) {
        assert(sym);  // nonnull
        symbol = sym;
    }

    Symbol* getSymbol() {
        return symbol;
    }
    SymbolVariable* getSymbolVar() {
        return dynamic_cast<SymbolVariable*>(symbol);
    }
    TypeSymbol* getSymbolType() {
        return dynamic_cast<TypeSymbol*>(symbol);
    }
};


class AstExprIdentifier : public AstExpr {
public:
    std::string name;
    explicit AstExprIdentifier(std::string  name) : name(std::move(name)) {}
};

class AstExprLString : public AstExpr {
public:
    std::string str;
    AstExprLString(std::string str) : str(std::move(str)) {}
};

class AstExprLNumber : public AstExpr {
public:
    TokenType* typ;
    union num {
        float f32;
        double f64;
        int i32;
        long i64;
        unsigned int u32;
        unsigned long u64;
    } num;
    AstExprLNumber(TokenType* typ) : typ(typ) {}
};



class AstExprBinaryOp : public AstExpr {
public:
    AstExpr* lhs;
    AstExpr* rhs;
    TokenType* typ;

    AstExprBinaryOp(AstExpr* lhs, AstExpr* rhs, TokenType* typ) : lhs(lhs), rhs(rhs), typ(typ) {}
};

class AstExprUnaryOp : public AstExpr{
public:
    AstExpr* expr;
    bool post;  // post/pre  suffix/prefix
    TokenType* typ;

    AstExprUnaryOp(AstExpr* expr, bool post, TokenType* typ) : expr(expr), post(post), typ(typ) {}
};

class AstExprTriCond : public AstExpr {
public:
    AstExpr* cond;
    AstExpr* then;
    AstExpr* els;

    AstExprTriCond(AstExpr* cond, AstExpr* then, AstExpr* els) : cond(cond), then(then), els(els) {}
};

class AstExprFuncCall : public AstExpr {
public:
    AstExpr* expr;
    std::vector<AstExpr*> args;

    AstExprFuncCall(AstExpr* expr, std::vector<AstExpr*> args) : expr(expr), args(std::move(args)) {}
};

class AstExprLambda : public AstExpr {};

class AstExprMemberAccess : public AstExpr {
public:
    AstExpr* lhs;
    std::string memb;
    TokenType* typ;
    AstExprMemberAccess(AstExpr* lhs, std::string memb, TokenType* typ) : lhs(lhs), memb(std::move(memb)), typ(typ) {}


    static std::vector<std::string> namesExpand(AstExpr* name) {
        std::vector<std::string> names;
        namesExpand(&names, name);
        return names;
    }
    static std::string namesLast(AstExpr* name) {
        std::vector<std::string> names;
        namesExpand(&names, name);
        return names[names.size()-1];
    }

    static void namesExpand(std::vector<std::string>* out, AstExpr* a) {
        if (CAST(AstExprMemberAccess*)) {
            namesExpand(out, c->lhs);
            out->push_back(c->memb);
        } else if (CAST(AstExprIdentifier*)) {
            out->push_back(c->name);
        } else throw "Illegal namespace name tree";
    }
};

//class AstExprNew : public AstExpr {};
//
//class AstExprSizeOf : public AstExpr {};

class AstExprTypeCast : public AstExpr {
public:
    AstExpr* type;
    AstExpr* expr;

    AstExprTypeCast(AstExpr* type, AstExpr* expr) : type(type), expr(expr) {}
};

#endif //ETHERTIA_ASTEXPR_H
