//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_MACEDURE_H
#define ETHERTIA_MACEDURE_H

#include <ethertia/lang/compiler/CodeBuf.h>

#include "../compiler/IO.h"

class Macedure
{
public:
    inline static u8 MEM[1024];
    inline static u32 esp = 0;
    inline static u32 ebp = 0;

    static const u32 M_HEAP = 512;
    static const u32 M_LSTRING = 700;
    static const u32 M_STATIC = 900;


    static void push_i32(i32 i) {
        IO::st_32(&MEM[esp], i);
        esp += 4;
    }
    static i32 pop_i32() {
        esp -= 4;
        return IO::ld_32(&MEM[esp]);
    }

    static void push_8(u8 v) {
        MEM[esp] = v;
        ++esp;
    }
    static u8 pop_8() {
        --esp;
        return MEM[esp];
    }

    static void push_ptr(u32 p) {
        push_i32(p);
    }
    static u32 pop_ptr() {
        return pop_i32();
    }

    static void memcpy(u32 src, u32 dst, u16 len) {
        for (int i = 0; i < len; ++i) {
            MEM[dst+i] = MEM[src+i];
        }
    }

    static void run(CodeBuf* cbuf, u32 ebp) {
        u8* code = cbuf->buf.data();
        u32 ip = 0;

        esp = ebp;
        u32 locals[cbuf->localvars.size()];
        for (int i = 0; i < cbuf->localvars.size(); ++i) {
            locals[i] = esp;
            esp += cbuf->localvars[i]->getType()->getTypesize();
        }
        std::cout << Log::str("::PROC <ebp:{}, esp:{}>\n", ebp, esp);

        while (ip < cbuf->buf.size()) {

        std::string opstr = Opcodes::str(&code[ip]);
        if (code[ip] == Opcodes::VERBO) {
            u32 begp = ip;
            u8 len = code[ip+1]; ip+=2;
            std::stringstream ss; for (int i = 0; i < len; ++i) ss << code[ip++];
            std::cout << Log::str("#{5} {20}  ; % {}\n", begp, "VERBO", ss.str());
            continue;
        } else {
            std::stringstream strfs; for (int i = 0; i < esp-ebp; ++i) { IO::hex(strfs, MEM[ebp+i]); strfs << ' '; }
            std::cout << Log::str("#{5} {20}  ; {}\n", ip, IO::uppercase(opstr), strfs.str());
        }

        u8 opc = code[ip++];
        switch (opc) {
            case Opcodes::ADD_I32: {
                i32 rhs = pop_i32();
                i32 lhs = pop_i32();
                push_i32(lhs+rhs);
                break;
            }
            case Opcodes::LDL: {
                u8 lidx = code[ip++];
                push_ptr(locals[lidx]);
                break;
            }
            case Opcodes::LDC: {
                u8 typ = code[ip++];
                if (typ == Opcodes::LDC_U8)       { push_8(code[ip++]);                }
                else if (typ == Opcodes::LDC_I32) { push_i32(IO::ld_32(&code[ip])); ip+=4; }
                else { throw "Unsupported constant type"; }
                break;
            }
            case Opcodes::MOV_POP: {
                u16 tsize = IO::ld_16(&code[ip]); ip += 2;
                esp -= tsize;
                u32 src_ptr = esp;
                u32 dst_ptr = pop_ptr();
                memcpy(src_ptr, dst_ptr, tsize);
                break;
            }
            case Opcodes::POP: {
                u16 sz = IO::ld_16(&code[ip]); ip += 2;
                esp -= sz;
                break;
            }
            case Opcodes::DUP: {
                u16 sz = IO::ld_16(&code[ip]); ip += 2;
                memcpy(esp-sz, esp, sz);
                esp += sz;
            }
            case Opcodes::VERBO:
            case Opcodes::NOP: {
                break;
            }
            default: {
                throw Log::str("Unknown opcode: ", (int)opc);
            }
        }
        }
    }

};

#endif //ETHERTIA_MACEDURE_H
