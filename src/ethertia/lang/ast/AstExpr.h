//
// Created by Dreamtowards on 2022/5/4.
//

#ifndef ETHERTIA_ASTEXPR_H
#define ETHERTIA_ASTEXPR_H

#include <utility>

#include "Ast.h"

class AstExpr : public Ast {

};

class AstExprBinOp : public AstExpr{};

class AstExprUnaryOp : public AstExpr{};

class AstExprTriOp : public AstExpr{};

class AstExprFuncCall : public AstExpr{};

class AstExprTemplateFilled : public AstExpr {};

class AstExprLambda : public AstExpr {};

class AstExprMemberAccess : public AstExpr {
public:
    static constexpr int T_PATH = 1;
    static constexpr int T_DOT = 2;
    static constexpr int T_ARROW = 3;

    AstExpr* lhs;
    AstExpr* rhs;
    int type;
    AstExprMemberAccess(AstExpr *lhs, AstExpr *rhs, int type) : lhs(lhs), rhs(rhs), type(type) {}
};

class AstExprNew : public AstExpr {};

class AstExprSizeOf : public AstExpr {};

class AstExprTypeCast : public AstExpr {};

class AstExprIdentifier : public AstExpr {
public:
    std::string name;
    explicit AstExprIdentifier(std::string  name) : name(std::move(name)) {}
};

class AstExprLiteralConstant : public AstExpr {};



#endif //ETHERTIA_ASTEXPR_H
