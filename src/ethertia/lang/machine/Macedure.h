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

    static const t_ptr M_HEAP = 512;     // Heap Top. head address is descending increasing..
    static const t_ptr M_STATIC = M_HEAP;


    inline static u8 MEM[2048];
    inline static t_ptr esp = 0;
    inline static t_ptr ebp = 0;
    inline static t_ptr stp = M_STATIC;  // static storage pointer


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

    static void push_ptr(t_ptr p) {
        push_i32(p);
    }
    static t_ptr pop_ptr() {
        return pop_i32();
    }

    static void memcpy(t_ptr src, t_ptr dst, u16 len) {
        for (int i = 0; i < len; ++i) {
            MEM[dst+i] = MEM[src+i];
        }
    }

    static void run(t_ptr ip_ptr, const t_ptr ebp) {
        u8* code = &MEM[ip_ptr];
        t_ip ip = 0;

//        esp = ebp;
//        u32 locals[cbuf->localvars.size()];
//        for (int i = 0; i < cbuf->localvars.size(); ++i) {
//            locals[i] = esp;
//            esp += cbuf->localvars[i]->getType()->getTypesize();
//        }

        std::cout << Log::str("::PROC <ip_ptr: {}, ebp:{}, esp:{}>\n", ip_ptr, ebp, esp); int i100 = 0;

        while (true) {  //if (i100++ > 200) break;

        if (code[ip] == Opcodes::VERBO) {
            u32 begp = ip;
            u8 len = code[ip+1]; ip+=2;
            std::stringstream ss; for (int i = 0; i < len; ++i) ss << code[ip++];
            std::cout << Log::str("#{5} {20}  ; % {}\n", begp, "VERBO", ss.str());
            continue;
        } else {
            std::string opstr = Opcodes::str(&code[ip]);
            std::cout << Log::str("#{5} {20}  ; {}\n", ip, IO::uppercase(opstr), IO::dump(&MEM[ebp], esp-ebp));
        }

        u8 opc = code[ip++];
        switch (opc) {
            case Opcodes::ADD_I32: {
                i32 rhs = pop_i32();
                i32 lhs = pop_i32();
                push_i32(lhs+rhs);
                break;
            }
            case Opcodes::ICMP: {
                u8 cond = code[ip++];
                u8 typ  = code[ip++];
                if (typ == Opcodes::ICMP_I32) {
                    i32 rhs = pop_i32();
                    i32 lhs = pop_i32();
                    if (cond == Opcodes::ICMP_SGT) push_8(lhs > rhs);
                    else if (cond == Opcodes::ICMP_SLT) push_8(lhs < rhs);
                    else throw "unsupp icmp cond";
                } else {
                    throw "unsupp icmp typ";
                }
                break;
            }
            case Opcodes::LDL: {
                u16 lpos = IO::ld_16(&code[ip]); ip += 2;
                push_ptr(ebp + lpos);
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
            case Opcodes::MOV_PUSH: {
                u16 sz = IO::ld_16(&code[ip]); ip += 2;
                u32 src_ptr = pop_ptr();
                u32 dst_ptr = esp;
                memcpy(src_ptr, dst_ptr, sz);
                esp += sz;
                break;
            }
            case Opcodes::MOV: {
                u16 sz = IO::ld_16(&code[ip]); ip += 2;
                t_ptr src_ptr = pop_ptr();
                t_ptr dst_ptr = pop_ptr();
                memcpy(src_ptr, dst_ptr, sz);
                break;
            }
            case Opcodes::DUP: {
                u16 sz = IO::ld_16(&code[ip]); ip += 2;
                memcpy(esp-sz, esp, sz);
                esp += sz;
                break;
            }
            case Opcodes::POP: {
                u16 sz = IO::ld_16(&code[ip]); ip += 2;
                esp -= sz;
                break;
            }
            case Opcodes::PUSH: {
                u16 sz = IO::ld_16(&code[ip]); ip += 2;
                esp += sz;
                break;
            }
            case Opcodes::GOTO: {
                u16 dst_ip = IO::ld_16(&code[ip]); ip += 2;
                ip = dst_ip;
                break;
            }
            case Opcodes::GOTO_F: {
                u16 dst_ip = IO::ld_16(&code[ip]); ip += 2;
                u8 cond = pop_8();
                if (cond == 0) {
                    ip = dst_ip;
                }
                break;
            }
            case Opcodes::CALL: {
                u16 f_args_bytes = IO::ld_16(&code[ip]); ip += 2;
                u16 fpos = IO::ld_16(&code[ip]); ip += 2;

                t_ptr f_ebp = esp - f_args_bytes;
                t_ptr f_ip = M_STATIC + fpos;

                run(f_ip, f_ebp);

                break;
            }
            case Opcodes::VERBO:
            case Opcodes::NOP:
                break;
            case Opcodes::RET: {

                std::cout << Log::str("::END_PROC <ip_ptr: {}, ebp: {}, esp: {}>\n", ip_ptr, ebp, esp);
                return;
            }
            default: {
                throw Log::str("Unknown opcode: ", (int)opc);
            }
        }
        }
    }

};

// deprecate LDL, use EBP+N loadup.  scope-dependent local vars.
// return pop/dector

// function call, ??all use funcptr

#endif //ETHERTIA_MACEDURE_H
