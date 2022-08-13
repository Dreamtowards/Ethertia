//
// Created by Dreamtowards on 2022/6/4.
//

#include "SymbolInternalTypes.h"

SymbolInternalTypes SymbolInternalTypes::VOID{"void", 0};

SymbolInternalTypes SymbolInternalTypes::U8 {"u8",  1};
SymbolInternalTypes SymbolInternalTypes::U16{"u16", 2};
SymbolInternalTypes SymbolInternalTypes::U32{"u32", 4};
SymbolInternalTypes SymbolInternalTypes::U64{"u64", 8};

SymbolInternalTypes SymbolInternalTypes::I8 {"i8",  1};
SymbolInternalTypes SymbolInternalTypes::I16{"i16", 2};
SymbolInternalTypes SymbolInternalTypes::I32{"i32", 4};
SymbolInternalTypes SymbolInternalTypes::I64{"i64", 8};

SymbolInternalTypes SymbolInternalTypes::F32{"f32", 4};
SymbolInternalTypes SymbolInternalTypes::F64{"f64", 8};

SymbolInternalTypes SymbolInternalTypes::BOOL{"bool", 1};