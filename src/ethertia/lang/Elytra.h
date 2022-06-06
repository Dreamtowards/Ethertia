//
// Created by Dreamtowards on 2022/5/5.
//

#ifndef ETHERTIA_ELYTRA_H
#define ETHERTIA_ELYTRA_H

#include <ethertia/client/Loader.h>

#include <ethertia/lang/parser/Parser.h>
#include <ethertia/lang/symbol/Cymbal.h>
#include <ethertia/lang/symbol/SymbolInternalTypes.h>
#include <ethertia/lang/compiler/CodeGen.h>
#include <ethertia/lang/machine/Macedure.h>

void et() {

    Lexer lx;
    lx.src = Loader::loadAssetsStr("elytra/main.et");
    lx.src_name = "elytra/main.et";

    Log::info("stdc tellp", std::cout.tellp());

    // Parse Syntax, Lexical.
    AstCompilationUnit* a = Parser::parseCompilationUnit(&lx);

    {
        Scope rt(nullptr);
        SymbolInternalTypes::initInternalTypes(&rt);

        // Symbol
        Cymbal::visitCompilationUnit(&rt, a);
    }

    {
        for (auto& it : Cymbal::functions) {  AstStmtDefFunc* func = it.second;
            // CodeGen
            CodeGen::visitStmtDefFunc(&func->fsymbol->codebuf, func);
        }
    }

    {
        CodeBuf* cbuf = &Cymbal::functions["main"]->fsymbol->codebuf;

        // VM
        Macedure::run(cbuf, 4);
    }


}

#endif //ETHERTIA_ELYTRA_H
