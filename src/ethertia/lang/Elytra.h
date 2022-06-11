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


    Scope rt(nullptr);
    SymbolInternalTypes::initInternalTypes(&rt);

    // Symbol
    Cymbal::visitCompilationUnit(&rt, a);


    {
        SymbolFunction* sf = dynamic_cast<SymbolFunction*>(rt.resolve({"std", "_test", "_1", "main"}));
        CodeBuf::print(&sf->codebuf);

        // VM
        Macedure::run(Macedure::M_STATIC+sf->code_fpos, 0);
    }


}

/*
 * stmts: Labels, Goto
 */

#endif //ETHERTIA_ELYTRA_H
