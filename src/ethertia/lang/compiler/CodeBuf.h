//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_CODEBUF_H
#define ETHERTIA_CODEBUF_H

#include <map>

#include <ethertia/lang/compiler/Opcodes.h>

class CodeBuf
{
public:

    std::vector<SymbolVariable*> localvars;

    std::vector<u8> buf;


    void defvar(SymbolVariable* sv) {
        assert(ldvar(sv) == -1);

        localvars.push_back(sv);
    }
    int ldvar(SymbolVariable* sv) {
        for (int i = 0; i < localvars.size(); ++i) {
            if (localvars[i]->getSimpleName() == sv->getSimpleName())
                return i;
        }
        return -1;
    }






    void _ldl(SymbolVariable* sv) {
        _ldl(ldvar(sv));
    }
    void _ldl(u8 lidx) {
        cpush8(Opcodes::LDL);
        cpush8(lidx);
    }

    void _mov(u16 size) {
        cpush8(Opcodes::MOV);
        cpush16(size);
    }
    void _mov_pop(u16 size) {
        cpush8(Opcodes::MOV_POP);
        cpush16(size);
    }

    void _ldc_(u8 type) {
        cpush8(Opcodes::LDC);
        cpush8(type);
        // And Data
    }
    void _ldc_i32(i32 i) {
        _ldc_(Opcodes::LDC_I32);
        cpush32(i);
    }


    void _pop(u16 size) {
        cpush8(Opcodes::POP);
        cpush16(size);
    }


    void _add_i32() {
        cpush8(Opcodes::ADD_I32);
    }

    void _dup(u16 sz) {
        cpush8(Opcodes::DUP);
        cpush16(sz);
    }
    void _dup_ptr() {
        _dup(4);
    }


    void _verbo(std::string_view s) {
        cpush8(Opcodes::VERBO);
        cpush8(s.length());
        for (char i : s) {
            cpush8(i);
        }
    }

    void _nop() {
        cpush8(Opcodes::NOP);
    }

    // append code
    void cpush8(u8 v) {
        buf.push_back(v);
    }
    void cpush16(u16 v) {
        buf.push_back(v >> 8);
        buf.push_back(v);
    }
    void cpush32(u32 v) {
        buf.push_back(v >> 24);
        buf.push_back(v >> 16);
        buf.push_back(v >> 8);
        buf.push_back(v);
    }
};

#endif //ETHERTIA_CODEBUF_H
