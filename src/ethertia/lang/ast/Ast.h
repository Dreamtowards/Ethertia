//
// Created by Dreamtowards on 2022/5/4.
//

#ifndef ETHERTIA_AST_H
#define ETHERTIA_AST_H

#include <vector>

class Srcloc
{
public:
    const char* src = nullptr;
    int beg = 0;
    int end = 0;
    const char* src_name = nullptr;  // CompilationUnit name
};

class Ast
{
    virtual void vir() {};

public:
    Srcloc srcloc{};

    void initlc(int beg, Lexer* lx) {
        srcloc.beg = beg;
        srcloc.end = lx->rdi;
        srcloc.src = lx->src.c_str();
        srcloc.src_name = lx->src_name.c_str();
    }

    std::string_view str() const {
        return std::string_view(srcloc.src+srcloc.beg, srcloc.end-srcloc.beg);
    }
};


#define CAST(type) auto* c = dynamic_cast<type>(a)

#endif //ETHERTIA_AST_H
