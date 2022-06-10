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
#include <ethertia/lang/ast/prt/AstPrinter.h>

void et() {


    Lexer lx;
    lx.src = Loader::loadAssetsStr("elytra/main.et");
    lx.src_name = "elytra/main.et";

    Log::info("stdc tellp", std::cout.tellp());

    // Parse Syntax, Lexical.
    AstCompilationUnit* a = Parser::parseCompilationUnit(&lx);

    AstPrinter::Prt prt;
    AstPrinter::printCompilationUnit(prt, a);
    Log::info("Prt: \n", prt.ss.str());

    {
        Scope rt(nullptr);
        SymbolInternalTypes::initInternalTypes(&rt);

        // Symbol
        Cymbal::visitCompilationUnit(&rt, a);
    }

    {
        for (auto& it : Cymbal::functions) {  AstStmtDefFunc* func = it.second;
            // CodeGen
            CodeBuf* cbuf = &func->fsymbol->codebuf;
            CodeGen::visitStmtDefFunc(cbuf, func);

            int len = cbuf->bufpos();
            memcpy(&Macedure::MEM[Macedure::stp], cbuf->bufptr(0), len);
            Macedure::stp += len;
        }
    }


    {
        CodeBuf* cbuf = &Cymbal::functions["main"]->fsymbol->codebuf;

        u32 ip = 0;
        while (ip < cbuf->buf.size()) {
            u8 step;
            std::cout << Log::str("#{5}  ", ip) << Opcodes::str(&cbuf->buf[ip], &step) << "\n";
            ip += step;
        }
        std::cout << "END ASM.\n\n\n";

        // VM
        Macedure::run(512, 0);
    }


}

/*
 * stmts: Labels, Goto
 */

#endif //ETHERTIA_ELYTRA_H
