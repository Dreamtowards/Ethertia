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
#include <cxxtest/TestSuite.h>
#include "io/izlibstream.h"
#include "io/ozlibstream.h"
#include <fstream>
#include <sstream>

using namespace zlib;

class zlibstream_test : public CxxTest::TestSuite
{
private:
    std::string bigtest;

public:
    zlibstream_test()
    {
        std::ifstream bigtest_f("bigtest_uncompr", std::ios::binary);
        std::stringbuf bigtest_b;
        bigtest_f >> &bigtest_b;
        bigtest = bigtest_b.str();
        if(!bigtest_f || bigtest.size() == 0)
            throw std::runtime_error("Could not read bigtest_uncompr file");
    }

    void test_inflate_gzip()
    {
        std::ifstream gzip_in("bigtest.nbt", std::ios::binary);
        TS_ASSERT(gzip_in);

        std::stringbuf data;
        //Small buffer so not all fits at once (the compressed file is 561 bytes)
        {
            izlibstream igzs(gzip_in, 256);
            igzs.exceptions(std::ios::failbit | std::ios::badbit);
            TS_ASSERT(igzs.is_open());
            TS_ASSERT(igzs.good());

            TS_ASSERT_THROWS_NOTHING(igzs >> &data);
            TS_ASSERT(igzs);
            TS_ASSERT(igzs.eof());
            TS_ASSERT_EQUALS(data.str(), bigtest);

            TS_ASSERT(igzs.is_open());
        }

        //Clear and reuse buffers
        data.str("");
        gzip_in.clear();
        gzip_in.seekg(0);
        //Now try the same with larger buffer (but not large enough for all output, uncompressed size 1561 bytes)
        {
            izlibstream igzs(gzip_in, 1000);
            igzs.exceptions(std::ios::failbit | std::ios::badbit);
            TS_ASSERT(igzs.good());

            TS_ASSERT_THROWS_NOTHING(igzs >> &data);
            TS_ASSERT(igzs);
            TS_ASSERT(igzs.eof());
            TS_ASSERT_EQUALS(data.str(), bigtest);
        }

        data.str("");
        gzip_in.clear();
        gzip_in.seekg(0);
        //Now with large buffer
        {
            izlibstream igzs(gzip_in, 4000);
            igzs.exceptions(std::ios::failbit | std::ios::badbit);
            TS_ASSERT(igzs.good());

            TS_ASSERT_THROWS_NOTHING(igzs >> &data);
            TS_ASSERT(igzs);
            TS_ASSERT(igzs.eof());
            TS_ASSERT_EQUALS(data.str(), bigtest);
        }
    }

    void test_inflate_zlib()
    {
        std::ifstream zlib_in("bigtest.zlib", std::ios::binary);
        TS_ASSERT(zlib_in);

        std::stringbuf data;
        izlibstream izls(zlib_in, 256);
        izls.exceptions(std::ios::failbit | std::ios::badbit);
        TS_ASSERT(izls.good());
        TS_ASSERT(izls.is_open());

        TS_ASSERT_THROWS_NOTHING(izls >> &data);
        TS_ASSERT(izls);
        TS_ASSERT(izls.eof());
        TS_ASSERT_EQUALS(data.str(), bigtest);

        TS_ASSERT(izls.is_open());
    }

    void test_inflate_corrupt()
    {
        std::ifstream gzip_in("bigtest_corrupt.nbt", std::ios::binary);
        TS_ASSERT(gzip_in);

        std::vector<char> buf(bigtest.size());

        izlibstream igzs(gzip_in);
        igzs.exceptions(std::ios::failbit | std::ios::badbit);
        TS_ASSERT_THROWS(igzs.read(buf.data(), buf.size()), zlib_error);
        TS_ASSERT(igzs.bad());
        TS_ASSERT(igzs.is_open());

        //Close and reopen the streams
        gzip_in.close();
        gzip_in.clear();
        gzip_in.open("bigtest_eof.nbt", std::ios::binary);
        TS_ASSERT(gzip_in);

        TS_ASSERT_THROWS_NOTHING(igzs.reset());
        TS_ASSERT(igzs.good());
        TS_ASSERT(igzs.is_open());

        TS_ASSERT_THROWS(igzs.read(buf.data(), buf.size()), zlib_error);
        TS_ASSERT(igzs.bad());
        TS_ASSERT(igzs.is_open());
    }

    void test_inflate_trailing()
    {
        //This file contains additional uncompressed data after the zlib-compressed data
        std::ifstream file("trailing_data.zlib", std::ios::binary);
        izlibstream izls(file, 32);
        TS_ASSERT(file && izls);

        std::string str;
        izls >> str;
        TS_ASSERT(izls);
        TS_ASSERT(izls.eof());
        TS_ASSERT(izls.is_open());
        TS_ASSERT_EQUALS(str, "foobar");

        //Now read the uncompressed data
        TS_ASSERT(file);
        TS_ASSERT(!file.eof());
        file >> str;
        TS_ASSERT(!file.bad());
        TS_ASSERT_EQUALS(str, "barbaz");
    }

    void test_inflate_open()
    {
        std::ifstream zlib_in("bigtest.zlib", std::ios::binary);
        std::stringbuf data;

        izlibstream izls(zlib_in);
        izls.exceptions(std::ios::failbit | std::ios::badbit);
        TS_ASSERT(izls.good());
        TS_ASSERT(izls.is_open());
        izls >> &data;

        TS_ASSERT_THROWS_NOTHING(izls.close());
        TS_ASSERT(izls);
        TS_ASSERT(izls.eof());
        TS_ASSERT(!izls.is_open());

        data.str("");
        zlib_in.clear();
        zlib_in.seekg(0);

        TS_ASSERT_THROWS_NOTHING(izls.open());
        TS_ASSERT(izls.good());
        TS_ASSERT(!izls.eof());
        TS_ASSERT(izls.is_open());

        TS_ASSERT_THROWS_NOTHING(izls >> &data);
        TS_ASSERT(izls);
        TS_ASSERT_EQUALS(data.str(), bigtest);
        TS_ASSERT(izls.is_open());

        data.str("");
        zlib_in.clear();
        zlib_in.seekg(0);

        TS_ASSERT(izls.eof());
        TS_ASSERT_THROWS_NOTHING(izls.reset());
        TS_ASSERT(izls.good());
        TS_ASSERT(!izls.eof());
        TS_ASSERT(izls.is_open());

        TS_ASSERT_THROWS_NOTHING(izls >> &data);
        TS_ASSERT(izls);
        TS_ASSERT_EQUALS(data.str(), bigtest);
        TS_ASSERT(izls.is_open());

        TS_ASSERT_THROWS_NOTHING(izls.close());
        TS_ASSERT(izls);
        TS_ASSERT(!izls.is_open());

        TS_ASSERT_THROWS_ANYTHING(izls >> &data);
        TS_ASSERT(!izls);
    }

    void test_deflate_zlib()
    {
        //Here we assume that inflating works and has already been tested
        std::stringstream str;
        std::stringbuf output;
        //Small buffer
        {
            ozlibstream ozls(str, -1, false, 256);
            ozls.exceptions(std::ios::failbit | std::ios::badbit);
            TS_ASSERT(ozls.is_open());
            TS_ASSERT_THROWS_NOTHING(ozls << bigtest);
            TS_ASSERT(ozls.good());
            TS_ASSERT(ozls.is_open());
            TS_ASSERT_THROWS_NOTHING(ozls.close());
            TS_ASSERT(ozls.good());
            TS_ASSERT(!ozls.is_open());
        }
        TS_ASSERT(str.good());
        {
            izlibstream izls(str);
            TS_ASSERT_THROWS_NOTHING(izls >> &output);
            TS_ASSERT(izls);
        }
        TS_ASSERT_EQUALS(output.str(), bigtest);

        str.clear(); str.str("");
        output.str("");
        //Medium sized buffer
        //Write first half, then flush and write second half
        {
            ozlibstream ozls(str, 9, false, 512);
            ozls.exceptions(std::ios::failbit | std::ios::badbit);

            std::string half1 = bigtest.substr(0, bigtest.size()/2);
            std::string half2 = bigtest.substr(bigtest.size()/2);
            TS_ASSERT_THROWS_NOTHING(ozls << half1 << std::flush << half2);
            TS_ASSERT(ozls.good());
            TS_ASSERT_THROWS_NOTHING(ozls.close());
            TS_ASSERT(ozls.good());
        }
        TS_ASSERT(str.good());
        {
            izlibstream izls(str);
            izls >> &output;
            TS_ASSERT(izls);
        }
        TS_ASSERT_EQUALS(output.str(), bigtest);

        str.clear(); str.str("");
        output.str("");
        //Large buffer
        {
            ozlibstream ozls(str, 1, false, 4000);
            ozls.exceptions(std::ios::failbit | std::ios::badbit);
            TS_ASSERT_THROWS_NOTHING(ozls << bigtest);
            TS_ASSERT(ozls.good());
            TS_ASSERT_THROWS_NOTHING(ozls.close());
            TS_ASSERT(!ozls.is_open());
            TS_ASSERT_THROWS_NOTHING(ozls.close()); //closing twice shouldn't be a problem
            TS_ASSERT(ozls.good());
            TS_ASSERT(!ozls.is_open());
        }
        TS_ASSERT(str.good());
        {
            izlibstream izls(str);
            izls >> &output;
            TS_ASSERT(izls);
        }
        TS_ASSERT_EQUALS(output.str(), bigtest);
    }

    void test_deflate_gzip()
    {
        std::stringstream str;
        std::stringbuf output;
        {
            ozlibstream ozls(str, -1, true);
            ozls.exceptions(std::ios::failbit | std::ios::badbit);
            TS_ASSERT_THROWS_NOTHING(ozls << bigtest);
            TS_ASSERT(ozls.good());
            TS_ASSERT_THROWS_NOTHING(ozls.close());
            TS_ASSERT(ozls.good());
        }
        TS_ASSERT(str.good());
        {
            izlibstream izls(str);
            izls >> &output;
            TS_ASSERT(izls);
        }
        TS_ASSERT_EQUALS(output.str(), bigtest);
    }

    void test_deflate_open()
    {
        std::stringstream str;

        ozlibstream ozls(str);
        ozls << bigtest;

        TS_ASSERT_THROWS_NOTHING(ozls.close());
        std::string compr_bigtest = str.str();
        str.str("");

        TS_ASSERT_THROWS_NOTHING(ozls.open());
        TS_ASSERT(ozls.is_open());

        TS_ASSERT_THROWS_NOTHING(ozls.open());
        TS_ASSERT(!ozls);
        TS_ASSERT(ozls.is_open());
        ozls.clear();

        ozls << bigtest;
        TS_ASSERT(ozls.good());

        TS_ASSERT_THROWS_NOTHING(ozls.reset());
        TS_ASSERT(ozls.good());
        TS_ASSERT(ozls.is_open());
        TS_ASSERT_EQUALS(str.str(), compr_bigtest);
        str.str("");

        ozls << bigtest;
        TS_ASSERT_THROWS_NOTHING(ozls.close());
        TS_ASSERT(ozls.good());
        TS_ASSERT_EQUALS(str.str(), compr_bigtest);

        TS_ASSERT_THROWS_NOTHING(ozls.reset());
        TS_ASSERT(!ozls);
        TS_ASSERT(!ozls.is_open());
    }

    void test_deflate_closed()
    {
        std::stringstream str;
        {
            ozlibstream ozls(str);
            ozls.exceptions(std::ios::failbit | std::ios::badbit);
            TS_ASSERT_THROWS_NOTHING(ozls << bigtest);
            TS_ASSERT_THROWS_NOTHING(ozls.close());
            TS_ASSERT_THROWS_NOTHING(ozls << "foo");
            TS_ASSERT(!ozls.is_open());
            TS_ASSERT_THROWS_ANYTHING(ozls.flush());
            TS_ASSERT(ozls.bad());
            TS_ASSERT(!ozls.is_open());
            TS_ASSERT(!str);
        }
        TS_ASSERT(!str);
        str.clear();
        str.seekp(0);
        {
            ozlibstream ozls(str);
            //this time without exceptions
            TS_ASSERT_THROWS_NOTHING(ozls << bigtest);
            TS_ASSERT_THROWS_NOTHING(ozls.close());
            TS_ASSERT_THROWS_NOTHING(ozls << "foo" << std::flush);
            TS_ASSERT(ozls.bad());
            TS_ASSERT(!ozls.is_open());
            TS_ASSERT_THROWS_NOTHING(ozls.close());
            TS_ASSERT(ozls.bad());
            TS_ASSERT(!ozls.is_open());
            TS_ASSERT(!str);
        }
    }
};
