//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_OPCODES_H
#define ETHERTIA_OPCODES_H

#include "IO.h"

class Opcodes
{
public:

    static const u8 IADD    = 1,   //                  // u8 type
                    ICMP    = 2,

                    MOV     = 10,  // cpy only.        // u16 size; usize src_ptr, usize dst_ptr
                    DUP     = 11,  // dup stack top    // u16 size;
                    STV     = 12,  // pop->addr        // u16 size; usize dst_ptr
                    POP     = 13,  // just sub esp.    // u16 size;
                    PUSH    = 14,  // just add esp     // u16 size;
                    NOP     = 15,  // none,

                    LDL     = 20,  // load local var.  // u16 lpos.    // %ebp+lpos
                    LDC     = 21,  // load constant    // u8 type, u8[] data
                    LDS     = 22,  // load static pos  // u16 spos.    // reside of runtime-static-base
                    LDV     = 23,  // load ptr val     // u16 tsize

                    VERBO   = 30,  // debug comment    // u8 strlen u8[] str_ascii

                    JMP     = 40,
                    JMP_F   = 41,  // goto if false    //
                    CALL    = 45,                      // u16 args_size, u8[] fname
                    RET     = 46,  // terminate exec. since opcode have no boundary but just exec-pointer, need a code to do terminate.
                    CALLI   = 47;  // calli internal   // u16 args_size, u8* fname


    // LDC constant types.
    static const u8 LDC_I8  = 1,
                    LDC_I16 = 2,
                    LDC_I32 = 3,
                    LDC_I64 = 4,
                    LDC_F32 = 5,
                    LDC_F64 = 6;

    // ICMP conds
    static const u8 ICMP_EQ  = 1,
                    ICMP_NE  = 2,
                    ICMP_UGT = 3,  // unsigned greater
                    ICMP_UGE = 4,  // unsigned greater equals
                    ICMP_ULT = 5,
                    ICMP_ULE = 6,
                    ICMP_SGT = 7,
                    ICMP_SGE = 8,
                    ICMP_SLT = 9,
                    ICMP_SLE = 10;
    // ICMP num lens
    static const u8 T_I8  = 1,
                    T_I16 = 2,
                    T_I32 = 3,
                    T_I64 = 4;

    static std::string str(u8* p) {
        u8 stp;
        return str(p, &stp);
    }
    static std::string str(u8* p, u8* stp) {
        switch (p[0]) {
            case MOV:     *stp = 3; return Log::str("mov %{}", IO::ld_16(&p[1]));
            case STV:     *stp = 3; return Log::str("stv %{}", IO::ld_16(&p[1]));
            case LDV:     *stp = 3; return Log::str("ldv %{}", IO::ld_16(&p[1]));
            case POP:     *stp = 3; return Log::str("pop %{}", IO::ld_16(&p[1]));
            case PUSH:    *stp = 3; return Log::str("push %{}", IO::ld_16(&p[1]));
            case DUP:     *stp = 3; return Log::str("dup %{}", IO::ld_16(&p[1]));
            case NOP:     *stp = 1; return "nop";
            case RET:     *stp = 1; return "ret";
            case LDL:     *stp = 3; return Log::str("ldl ${}", IO::ld_16(&p[1]));
            case LDS:     *stp = 3; return Log::str("lds +${}", IO::ld_16(&p[1]));
            case JMP:    *stp = 3; return Log::str("jmp #{}", IO::ld_16(&p[1]));
            case JMP_F:  *stp = 3; return Log::str("jmp_f #{}", IO::ld_16(&p[1]));
            case CALL:    *stp = 5; return Log::str("call %{} @{}", IO::ld_16(&p[1]), IO::ld_16(&p[3]));
            case CALLI: {
                u8 len = p[3];
                std::stringstream ss;
                for (int i = 0; i < len; ++i) ss << p[4+i];
                *stp = 4 + len;
                return Log::str("calli %{} @{}", IO::ld_16(&p[1]), ss.str());
            }
            case IADD: {
                *stp = 2;
                u8 t = p[1];
                const char* ts = "?";
                if (t == T_I8) ts = "i8";
                else if (t == T_I16) ts = "i16";
                else if (t == T_I32) ts = "i32";
                else if (t == T_I64) ts = "i64";
                return Log::str("iadd {}", ts);
            }
            case LDC: { u8 typ = p[1];
                if (typ == LDC_I8) {  *stp = 3; return Log::str("ldc i8 %{}", (int)p[2]); }
                else if (typ == LDC_I32) { *stp = 6; return Log::str("ldc i32 %{}", IO::ld_32(&p[2])); }
                else if (typ == LDC_I64) { *stp = 10; return Log::str("ldc i64 %{}", IO::ld_64(&p[2])); }
                else return "ldc ?";
            }
            case ICMP: { u8 cond = p[1];  u8 typ = p[2];   *stp = 3;
                const char* scond = "?";
                const char* styp = "?";
                if (cond == ICMP_SGT) scond = "sgt";
                else if (cond == ICMP_SGE) scond = "sge";
                else if (cond == ICMP_SLT) scond = "slt";
                if (typ == T_I32) styp = "i32";
                return Log::str("icmp @{} {}", scond, styp);
            }
            case VERBO: {
                u8 len = p[1];  *stp = 2+len;
                std::stringstream ss; ss << "verbo:  % ";
                for (int i = 0; i < len; ++i) { ss << p[2+i]; }
                return ss.str();
            }
            default:     return Log::str("unknown op {}", (int)p[0]);
        }
    }
};

#endif //ETHERTIA_OPCODES_H
