/*
 * libnbt++ - A library for the Minecraft Named Binary Tag format.
 * Copyright (C) 2013, 2015  ljfa-ag
 *
 * This file is part of libnbt++.
 *
 * libnbt++ is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * libnbt++ is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with libnbt++.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "nbt/io/ozlibstream.h"
#include "nbt/io/zlib_streambuf.h"

namespace zlib
{

deflate_streambuf::deflate_streambuf(std::ostream& output, size_t bufsize, int level, int window_bits, int mem_level, int strategy):
    zlib_streambuf(bufsize), os(output)
{
    open(level, window_bits, mem_level, strategy);
}

deflate_streambuf::~deflate_streambuf() noexcept
{
    try
    {
        close();
    }
    catch(...)
    {
        //ignore as we can't do anything about it
    }
    deflateEnd(&zstr);
}

void deflate_streambuf::open(int level, int window_bits, int mem_level, int strategy)
{
    if(is_open_)
        return;

    int ret = deflateInit2(&zstr, level, Z_DEFLATED, window_bits, mem_level, strategy);
    if(ret != Z_OK)
        throw zlib_error(zstr.msg, ret);
    is_open_ = true;
    setp(in.data(), in.data() + in.size());
}

void deflate_streambuf::close()
{
    if(is_open_)
        deflate_chunk(Z_FINISH);
    deflateEnd(&zstr);
    is_open_ = false;
}

void deflate_streambuf::reset()
{
    if(is_open_)
    {
        deflate_chunk(Z_FINISH);
        int ret = deflateReset(&zstr);
        if(ret != Z_OK)
            throw zlib_error(zstr.msg, ret);
        setp(in.data(), in.data() + in.size());
    }
}

void deflate_streambuf::deflate_chunk(int flush)
{
    zstr.next_in = reinterpret_cast<Bytef*>(pbase());
    zstr.avail_in = pptr() - pbase();
    do
    {
        zstr.next_out = reinterpret_cast<Bytef*>(out.data());
        zstr.avail_out = out.size();
        int ret = deflate(&zstr, flush);
        if(ret != Z_OK && ret != Z_STREAM_END)
        {
            os.setstate(std::ios_base::failbit);
            throw zlib_error(zstr.msg, ret);
        }
        int have = out.size() - zstr.avail_out;
        if(!os.write(out.data(), have))
            throw std::ios_base::failure("Could not write to the output stream");
    } while(zstr.avail_out == 0);
    setp(in.data(), in.data() + in.size());
}

deflate_streambuf::int_type deflate_streambuf::overflow(int_type ch)
{
    deflate_chunk();
    if(ch != traits_type::eof())
    {
        *pptr() = ch;
        pbump(1);
    }
    return ch;
}

int deflate_streambuf::sync()
{
    deflate_chunk();
    return 0;
}

void ozlibstream::open(int level, bool gzip)
{
    if(!is_open())
    {
        try
        {
            buf.open(level, 15 + (gzip ? 16 : 0));
        }
        catch(...)
        {
            setstate(badbit);
        }
    }
    else
        setstate(failbit);
}

void ozlibstream::close()
{
    try
    {
        buf.close();
    }
    catch(...)
    {
        setstate(badbit); //FIXME: This will throw the wrong type of exception
                          //but there's no good way of setting the badbit
                          //without causing an exception when exceptions is set
    }
}

void ozlibstream::reset()
{
    if(is_open())
    {
        try
        {
            buf.reset();
        }
        catch(...)
        {
            setstate(badbit);
        }
    }
    else
        setstate(failbit);
}

}
