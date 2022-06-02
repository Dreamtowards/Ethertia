//
// Created by Dreamtowards on 2022/5/4.
//

#ifndef ETHERTIA_ASTSTMT_H
#define ETHERTIA_ASTSTMT_H

#include <utility>

#include "Ast.h"

class AstStmt : public Ast
{

};

class AstExpr;


class AstStmtBlank : public AstStmt {};

class AstStmtBlock : public AstStmt {
public:
    std::vector<AstStmt*> stmts;
    AstStmtBlock(std::vector<AstStmt*> stmts) : stmts(std::move(stmts)) {}
};

class AstStmtExpr : public AstStmt {
public:
    AstExpr* expr;
    AstStmtExpr(AstExpr* expr) : expr(expr) {}
};


class AstStmtIf : public AstStmt {
public:
    AstExpr* cond;
    AstStmt* then;
    AstStmt* els;

    AstStmtIf(AstExpr* cond, AstStmt* then, AstStmt* els) : cond(cond), then(then), els(els) {}
};

class AstStmtWhile : public AstStmt {
public:
    AstExpr* cond;
    AstStmt* body;

    AstStmtWhile(AstExpr* cond, AstStmt* body) : cond(cond), body(body) {}
};

class AstStmtBreak : public AstStmt {};

class AstStmtContinue : public AstStmt {};

class AstStmtReturn : public AstStmt {
public:
    AstExpr* ret;
    AstStmtReturn(AstExpr *ret) : ret(ret) {}
};


class AstStmtDefClass : public AstStmt {
public:
    std::string name;
    std::vector<AstExpr*> superclasses;
    std::vector<AstStmt*> stmts;

    AstStmtDefClass(std::string name,
                    std::vector<AstExpr*> superclasses,
                    std::vector<AstStmt*> stmts) : name(std::move(name)), superclasses(std::move(superclasses)), stmts(std::move(stmts)) {}

};

class AstStmtDefVar : public AstStmt {
public:
    AstExpr* type;
    std::string name;
    AstExpr* init;
    AstStmtDefVar(AstExpr* type, std::string name, AstExpr* init) : type(type), name(std::move(name)), init(init) {}
};

class AstStmtDefFunc : public AstStmt {
public:
    AstExpr* retType;
    std::string name;
    std::vector<AstStmtDefVar*> params;
    AstStmtBlock* body;
    AstStmtDefFunc(AstExpr* retType,
                   std::string name,
                   std::vector<AstStmtDefVar*> params,
                   AstStmtBlock* body) : retType(retType), name(std::move(name)), params(std::move(params)), body(body) {}
};

class AstStmtUsing : public AstStmt {
public:
    AstExpr* refer;
    std::string alias;

    AstStmtUsing(AstExpr* refer, std::string alias) : refer(refer), alias(std::move(alias)) {}
};

class AstStmtNamespace : public AstStmt {
public:
    AstExpr* name;
    std::vector<AstStmt*> stmts;

    AstStmtNamespace(AstExpr* name, std::vector<AstStmt*> stmts) : name(name), stmts(std::move(stmts)) {}
};


#endif //ETHERTIA_ASTSTMT_H
