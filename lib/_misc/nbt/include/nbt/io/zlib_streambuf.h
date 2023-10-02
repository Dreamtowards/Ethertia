#ifndef ZLIB_STREAMBUF_H_INCLUDED
#define ZLIB_STREAMBUF_H_INCLUDED

#include <stdexcept>
#include <streambuf>
#include <vector>
#include <zlib.h>
#include "nbt_export.h"

namespace zlib
{

///Exception thrown in case zlib encounters a problem
class NBT_EXPORT zlib_error : public std::runtime_error
{
public:
    const int errcode;

    zlib_error(const char* msg, int errcode):
        std::runtime_error(msg
                         ? std::string(zError(errcode)) + ": " + msg
                         : zError(errcode)),
        errcode(errcode)
    {}
};

///Base class for deflate_streambuf and inflate_streambuf
class zlib_streambuf : public std::streambuf
{
public:
    ///@return true if the buffer's internal zlib data structure is initialized
    bool is_open() const { return is_open_; }

protected:
    std::vector<char> in;
    std::vector<char> out;
    z_stream zstr;
    bool is_open_;

    explicit zlib_streambuf(size_t bufsize):
        in(bufsize), out(bufsize), is_open_(false)
    {
        zstr.zalloc = Z_NULL;
        zstr.zfree = Z_NULL;
        zstr.opaque = Z_NULL;
    }
};

}

#endif // ZLIB_STREAMBUF_H_INCLUDED
