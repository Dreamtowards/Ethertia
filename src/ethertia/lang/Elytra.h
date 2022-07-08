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

void et() {//try{

    std::vector<std::pair<AstCompilationUnit*, std::string>> sources = {
            {nullptr, "elytra/lib/stdx/glfw/glfw3.et"},
            {nullptr, "elytra/lib/ethertia/client/Window.et"},
            // {nullptr, "elytra/lib/ethertia/util/Timer.et"},
            {nullptr, "elytra/main.et"},
    };

    // Parse AST.
    for (auto& s : sources) {
        std::string& src = s.second;

        Lexer lx;
        lx.src = Loader::loadAssetsStr(src);
        lx.src_name = src;

        // Parse Syntax, Lexical.
        AstCompilationUnit* a = Parser::parseCompilationUnit(&lx);
        s.first = a;

        Log::info("Prt: [{}] \n{}", src, AstPrinter::printCompilationUnit(a));
    }

    Scope rt(nullptr);
    SymbolInternalTypes::initInternalTypes(&rt);

    // Semantic
    for (auto& s : sources)
    {
        Cymbal::visitCompilationUnit(&rt, s.first);
    }

    // CodeGen
    {
        SymbolFunction* sf = dynamic_cast<SymbolFunction*>(rt.resolve(Strings::split("ethertia::client::Ethertia::run", "::")));
        CodeBuf::print(&sf->codebuf);

        // load code
        memcpy(Macedure::beg_static, Cymbal::sbuf, Cymbal::spos);

        // VM
        Macedure::run(Macedure::beg_static + sf->code_spos, Macedure::MEM);
    }

//    } catch (const char* err) {
//        Log::warn("ERR: ", err);
//    }
}

#endif //ETHERTIA_ELYTRA_H
