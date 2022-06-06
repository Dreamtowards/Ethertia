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
    const char* src_name = nullptr;  // CompilationUnit name
    int beg = 0;
    int end = 0;
    int row = 0;
    int col = 0;
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

        int row = 0, col = 0;
        for (int i = 0; i < srcloc.beg; ++i) {
            col++;
            if (srcloc.src[i] == '\n') { col = 0; row++; }
        }
        srcloc.row = row;
        srcloc.col = col;
    }

    std::string str_v() const {
        std::string_view plain(srcloc.src+srcloc.beg, srcloc.end-srcloc.beg);

        return Log::str("{24} #[{}:{}]", plain, srcloc.row+1, srcloc.col+1);
    }
};


#define CAST(type) auto* c = dynamic_cast<type>(a)

#endif //ETHERTIA_AST_H
