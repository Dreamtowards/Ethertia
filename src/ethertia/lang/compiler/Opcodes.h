//
// Created by Dreamtowards on 2022/6/3.
//

#ifndef ETHERTIA_OPCODES_H
#define ETHERTIA_OPCODES_H

class Opcodes
{
public:

    static const u8 ADD_I32 = 1,

                    MOV     = 10,  // cpy only.        // u16 size; usize src_ptr, usize dst_ptr
                    MOV_POP = 11,                      // u16 size; usize dst_ptr
                    POP     = 12,  // just sub esp.    // u16 size;

                    LDL     = 20,  // load local var.  // u8 l_idx  // ? %ebp+lpos
                    LDC     = 21;  // load constant    // u8 type, u8[] data


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

};

#endif //ETHERTIA_OPCODES_H
