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
#include "nbt/io/izlibstream.h"
#include "nbt/io/zlib_streambuf.h"

namespace zlib
{

inflate_streambuf::inflate_streambuf(std::istream& input, size_t bufsize, int window_bits):
    zlib_streambuf(bufsize), is(input), stream_end(false)
{
    zstr.next_in = Z_NULL;
    zstr.avail_in = 0;
    open(window_bits);
}

inflate_streambuf::~inflate_streambuf() noexcept
{
    inflateEnd(&zstr);
}

void inflate_streambuf::open(int window_bits)
{
    if(is_open_)
        return;

    int ret = inflateInit2(&zstr, window_bits);
    if(ret != Z_OK)
        throw zlib_error(zstr.msg, ret);
    is_open_ = true;
    stream_end = false;

    char* end = out.data() + out.size();
    setg(end, end, end);
}

void inflate_streambuf::close()
{
    if(is_open_)
        inflateEnd(&zstr);
    is_open_ = false;
}

void inflate_streambuf::reset()
{
    if(is_open_)
    {
        int ret = inflateReset(&zstr);
        if(ret != Z_OK)
            throw zlib_error(zstr.msg, ret);
        char* end = out.data() + out.size();
        setg(end, end, end);
    }
}

inflate_streambuf::int_type inflate_streambuf::underflow()
{
    if(gptr() < egptr())
        return traits_type::to_int_type(*gptr());
    else if(!is_open())
        return traits_type::eof();

    size_t have;
    do
    {
        //Read if input buffer is empty
        if(zstr.avail_in <= 0)
        {
            is.read(in.data(), in.size());
            if(is.bad())
                throw std::ios_base::failure("Input stream is bad");
            size_t count = is.gcount();
            if(count == 0 && !stream_end)
                throw zlib_error("Unexpected end of stream", Z_DATA_ERROR);

            zstr.next_in = reinterpret_cast<Bytef*>(in.data());
            zstr.avail_in = count;
        }

        zstr.next_out = reinterpret_cast<Bytef*>(out.data());
        zstr.avail_out = out.size();

        int ret = inflate(&zstr, Z_NO_FLUSH);
        have = out.size() - zstr.avail_out;
        switch(ret)
        {
        case Z_NEED_DICT:
        case Z_DATA_ERROR:
            throw zlib_error(zstr.msg, ret);

        case Z_MEM_ERROR:
            throw std::bad_alloc();

        case Z_STREAM_END:
            if(!stream_end)
            {
                stream_end = true;
                //In case we consumed too much, we have to rewind the input stream
                is.clear();
                is.seekg(-static_cast<std::streamoff>(zstr.avail_in), std::ios_base::cur);
            }
            if(have == 0)
                return traits_type::eof();
            break;
        }
    } while(have == 0);

    setg(out.data(), out.data(), out.data() + have);
    return traits_type::to_int_type(*gptr());
}

void izlibstream::open()
{
    if(!is_open())
    {
        try
        {
            buf.open();
            clear();
        }
        catch(...)
        {
            setstate(badbit);
        }
    }
    else
        setstate(failbit);
}

void izlibstream::close()
{
    if(is_open())
    {
        buf.close();
    }
    else
        setstate(failbit);
}

void izlibstream::reset()
{
    if(is_open())
    {
        try
        {
            buf.reset();
            clear();
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
