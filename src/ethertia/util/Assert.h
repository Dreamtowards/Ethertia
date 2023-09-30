
#pragma once

#include <iostream>
#include <cassert>

#ifndef NDEBUG
#define ET_DEBUG 1
#endif // !NDEBUG

#ifdef ET_DEBUG

#define ET_ASSERT(expr) \
    if (!(expr)) { \
        std::cerr << "Assertion failed: " << #expr << std::endl; \
        std::abort(); \
    }

#else

#define ET_ASSERT(expr)

#endif // ET_DEBUG



#define ET_ERROR(msg) throw std::runtime_error(msg);