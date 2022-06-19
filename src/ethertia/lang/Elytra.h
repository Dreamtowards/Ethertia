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

#include <ethertia/lang/parser/SymbolScanner.h>
#include "ethertia/lang/ast/AstVisitor.h"

void et() {


    Lexer lx;
    lx.src = Loader::loadAssetsStr("elytra/main.et");
    lx.src_name = "elytra/main.et";


    // Parse Syntax, Lexical.
    AstCompilationUnit* a = Parser::parseCompilationUnit(&lx);


    Scope rt(nullptr);
    SymbolInternalTypes::initInternalTypes(&rt);

    Log::info("Prt: \n", AstPrinter::printCompilationUnit(a));


    // Symbol
    Cymbal::visitCompilationUnit(&rt, a);


    {
        SymbolFunction* sf = dynamic_cast<SymbolFunction*>(rt.resolve(Strings::split("ethertia::client::Ethertia::run", "::")));
        CodeBuf::print(&sf->codebuf);

        memcpy(&Macedure::MEM[Macedure::M_STATIC], Cymbal::sbuf, Cymbal::spos);

        // VM
        Macedure::run(Macedure::M_STATIC + sf->code_spos, 0);
    }


}

/*
 * stmts: Labels, Goto
 */

#endif //ETHERTIA_ELYTRA_H
