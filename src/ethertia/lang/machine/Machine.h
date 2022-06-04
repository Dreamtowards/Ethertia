//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_MACHINE_H
#define ETHERTIA_MACHINE_H

#include <ethertia/lang/compiler/CodeBuf.h>

class Machine
{
public:
    inline static u8 MEM[1024];
    inline static u32 esp = 0;

    static const u32 M_HEAP = 512;
    static const u32 M_LSTRING = 700;
    static const u32 M_STATIC = 900;

    static u32 ld_32(const u8* p) {
        return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    }
    static void st_32(u8* p, u32 v) {
        p[0] = v >> 24;
        p[1] = v >> 16;
        p[2] = v >> 8;
        p[3] = v;
    }

    static void push_i32(i32 i) {
        st_32(&MEM[esp], i);
        esp += 4;
    }
    static i32 pop_i32() {
        esp -= 4;
        return ld_32(&MEM[esp]);
    }

    static void run(CodeBuf* cbuf) {
        u8* code = cbuf->buf.data();
        u32 ip = 0;

        while (true) { u8 opc = code[ip++];
        switch (opc) {
            case Opcodes::ADD_I32: {
                i32 rhs = pop_i32();
                i32 lhs = pop_i32();
                push_i32(lhs+rhs);
                break;
            }
            default: {
                throw Log::str("Unknown opcode: ", (int)opc);
            }
        }
        }
    }

};

#endif //ETHERTIA_MACHINE_H
