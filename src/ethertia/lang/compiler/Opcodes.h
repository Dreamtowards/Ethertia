//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_OPCODES_H
#define ETHERTIA_OPCODES_H

#include "IO.h"

class Opcodes
{
public:

    static const u8 ADD_I32 = 1,

                    MOV     = 10,  // cpy only.        // u16 size; usize src_ptr, usize dst_ptr
                    MOV_POP = 11,                      // u16 size; usize dst_ptr
                    POP     = 12,  // just sub esp.    // u16 size;

                    LDL     = 20,  // load local var.  // u8 l_idx  // ? %ebp+lpos
                    LDC     = 21,  // load constant    // u8 type, u8[] data

                    VERBO   = 30;  // debug comment    // u8 strlen u8[] str_ascii


    static const u8 LDC_U8  = 1,
                    LDC_U16 = 2,
                    LDC_U32 = 3,
                    LDC_U64 = 4,
                    LDC_I8  = 5,
                    LDC_I16 = 6,
                    LDC_I32 = 7,
                    LDC_I64 = 8,
                    LDC_F32 = 9,
                    LDC_F64 = 10;

    static std::string str(u8* p) {
        u8 op = p[0];
        switch (op) {
            case ADD_I32: return Log::str("add_i32");
            case MOV:     return Log::str("mov %{}", IO::ld_16(&p[1]));
            case MOV_POP: return Log::str("mov_pop %{}", IO::ld_16(&p[1]));
            case POP:     return Log::str("pop %{}", IO::ld_16(&p[1]));
            case LDL:     return Log::str("ldl ${}", (int)p[1]);
            case LDC: { u8 typ = p[1];
                if (typ == LDC_I32) return Log::str("ldc i32 {}", IO::ld_32(&p[2]));
                else return "ldc ?";
            }
            case VERBO: {
                u8 len = p[1];
                std::stringstream ss;
                ss << "verbo  // ";
                for (int i = 0; i < len; ++i) {
                    ss << p[1+i];
                }
                return ss.str();
            }
            default:     return "unknown op";
        }
    }
};

#endif //ETHERTIA_OPCODES_H
