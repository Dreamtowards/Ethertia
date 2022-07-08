//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_MACEDURE_H
#define ETHERTIA_MACEDURE_H

#include <ethertia/lang/compiler/CodeBuf.h>

#include "../compiler/IO.h"

#include <dlfcn.h>

class Macedure
{
public:



    inline static u8 MEM[2048];
    inline static u8* esp = MEM;

    inline static u8* const beg_heap = MEM + 512;     // Heap Top. head address is descending increasing..
    inline static u8* const beg_static = beg_heap;

    // inline static t_ptr stp = M_STATIC;  // static storage pointer

    static void push_i64(i64 i) {
        IO::st_64(esp, i);
        esp += 8;
    }
    static u64 pop_i64() {
        esp -= 8;
        return IO::ld_64(esp);
    }

    static void push_i32(i32 i) {
        IO::st_32(esp, i);
        esp += 4;
    }
    static i32 pop_i32() {
        esp -= 4;
        return IO::ld_32(esp);
    }

    static void push_8(u8 v) {
        *esp = v;
        ++esp;
    }
    static u8 pop_8() {
        --esp;
        return *esp;
    }

    static void push_ptr(const u8* p) {
        push_i64((u64)p);
    }
    static u8* pop_ptr() {
        return (u8*)pop_i64();
    }

//    static void memcpy(t_ptr src, t_ptr dst, u16 len) {
//        for (int i = 0; i < len; ++i) {
//            MEM[dst+i] = MEM[src+i];
//        }
//    }


//    inline static std::map<std::string, void*> _libs;
//    inline static std::map<std::string, std::function<void>> _proc;
//
//    static void* _librarylookup(char* libname, char* fname) {
//        auto it = _libs.find(libname);
//        void* lib_handle = nullptr;
//        if (it == _libs.end()) {
//            lib_handle = dlopen(libname, RTLD_LAZY);
//            _libs[libname] = lib_handle;
//        } else {
//            lib_handle = it->second;
//        }
//
//        void* f = dlsym(lib_handle, fname);
//    }

    static u16 ld_16(u8*& ip) {
        u16 v = IO::ld_16(ip);
        ip += 2;
        return v;
    }
    static std::string ld_str(u8*& ip) {
        u8 len = *ip++;//  ld_16(ip);

        std::stringstream ss;
        for (int i = 0; i < len; ++i)
            ss << *ip++;
        return ss.str();
    }

    static void run(u8* base_ip, const u8* ebp) {
        u8* ip = base_ip;

        // libglfw.so glfwWindowHint 3 2

//        esp = ebp;
//        u32 locals[cbuf->localvars.size()];
//        for (int i = 0; i < cbuf->localvars.size(); ++i) {
//            locals[i] = esp;
//            esp += cbuf->localvars[i]->getType()->getTypesize();
//        }

        std::cout << Log::str("::PROC <ip_ptr: {}, ebp:{}, esp:+{}>\n", (void*)base_ip, (void*)ebp, (int)(esp-ebp)); int i100 = 0;

        while (true) {  //if (i100++ > 200) break;
        t_ptr rip = ip - base_ip;

        if (*ip == Opcodes::VERBO) {
            ip++;
            std::string vb = ld_str(ip);
//            u8 len = ip[1]; ip+=2;
//            std::stringstream ss; for (int i = 0; i < len; ++i) ss << *ip++;
            std::cout << Log::str("#{5} {40} ; % {}\n", rip, "VERBO", vb);
            continue;
        } else {
            std::string opstr = Opcodes::str(ip);
            std::cout << Log::str("#{5} {40} ; {}\n", ip-base_ip, IO::uppercase(opstr, ' '), IO::dump(ebp, esp-ebp));
        }

        u8 opc = *ip++;
        switch (opc) {
            case Opcodes::IADD: {
                u8 typ  = *ip++;
                if (typ == Opcodes::T_I32) {
                    i32 rhs = pop_i32();
                    i32 lhs = pop_i32();
                    push_i32(lhs+rhs);
                } else if (typ == Opcodes::T_I64) {
                    i64 rhs = pop_i64();
                    i64 lhs = pop_i64();
                    push_i64(lhs+rhs);
                }
                break;
            }
            case Opcodes::ICMP: {
                u8 cond = *ip++;
                u8 typ  = *ip++;
                if (typ == Opcodes::T_I32) {
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
                u16 lpos = ld_16(ip);
                push_ptr(ebp + lpos);
                break;
            }
            case Opcodes::LDC: {
                u8 typ = *ip++;
                if (typ == Opcodes::LDC_I8)       { push_8(*ip++);                }
                else if (typ == Opcodes::LDC_I32) { push_i32(IO::ld_32(ip)); ip+=4; }
                else if (typ == Opcodes::LDC_I64) { push_i64(IO::ld_64(ip)); ip+=8; }
                else { throw "Unsupported constant type"; }
                break;
            }
            case Opcodes::LDS: {
                u16 spos = ld_16(ip);
                push_ptr(beg_static+spos);
                break;
            }
            case Opcodes::LDV: {
                u16 sz = ld_16(ip);
                u8* src_ptr = pop_ptr();
                memcpy(esp, src_ptr, sz);
                esp += sz;
                break;
            }
            case Opcodes::STV: {
                u16 tsize = ld_16(ip);
                esp -= tsize;
                u8* src_ptr = esp;
                u8* dst_ptr = pop_ptr();
                memcpy(dst_ptr, src_ptr, tsize);
                break;
            }
            case Opcodes::MOV: {
                u16 sz = ld_16(ip);
                u8* src_ptr = pop_ptr();
                u8* dst_ptr = pop_ptr();
                memcpy(dst_ptr, src_ptr, sz);
                break;
            }
            case Opcodes::DUP: {
                u16 sz = ld_16(ip);
                memcpy(esp, esp-sz, sz);
                esp += sz;
                break;
            }
            case Opcodes::POP: {
                u16 sz = ld_16(ip);
                esp -= sz;
                break;
            }
            case Opcodes::PUSH: {
                u16 sz = ld_16(ip);
                esp += sz;
                break;
            }
            case Opcodes::JMP: {
                u16 dst_ip = ld_16(ip);
                ip = base_ip + dst_ip;
                break;
            }
            case Opcodes::JMP_F: {
                u16 dst_ip = ld_16(ip);
                u8 cond = pop_8();
                if (cond == 0) {
                    ip = base_ip + dst_ip;
                }
                break;
            }
            case Opcodes::CALL: {
                u16 args_bytes = ld_16(ip);
                u16 fpos = ld_16(ip);

                u8* f_ebp = esp - args_bytes;
                u8* f_ip = const_cast<u8*>(beg_static + fpos);

                run(f_ip, f_ebp);

                break;
            }
            case Opcodes::CALLI: {
                u16 args_bytes = ld_16(ip);
                std::string fname = ld_str(ip);

                u8* f_ebp = esp - args_bytes;

                IO::st_32(f_ebp, 8);
                esp = f_ebp + 4;

                Log::info("Call Native ", fname);

                break;
            }
            case Opcodes::VERBO:
            case Opcodes::NOP:
                break;
            case Opcodes::RET: {

                std::cout << Log::str("::END_PROC <ip_ptr: {}, ebp:{}, esp:+{}>\n", (void*)base_ip, (void*)ebp, (int)(esp-ebp));
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
