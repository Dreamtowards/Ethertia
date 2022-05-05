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

class AstStmtBlock : public AstStmt {};

class AstStmtExpr : public AstStmt {};


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

class AstStmtReturn : public AstStmt {};


class AstStmtDefClass : public AstStmt {};

class AstStmtDefFunc : public AstStmt {};

class AstStmtDefVar : public AstStmt {};

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


class AstCompilationUnit : public Ast
{
    std::vector<AstStmt*> stmts;

public:
    explicit AstCompilationUnit(std::vector<AstStmt *> stmts) : stmts(std::move(stmts)) {}
};


#endif //ETHERTIA_ASTSTMT_H
