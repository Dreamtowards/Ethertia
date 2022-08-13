//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_LOG_H
#define ETHERTIA_LOG_H

#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <sys/time.h>
#include <sstream>

template<typename T>
static std::string lstr(T v) {
    std::stringstream ss;
    ss << v;
    return ss.str();
}


class Log
{
public:

    template<typename OUT>
    static void log(OUT& out, const std::string& pat) {
        out << pat;
    }

    template<typename OUT, typename A, typename... ARGS>
    static void log(OUT& out, const std::string& pat, A a, ARGS... args) {
        int beg = pat.find('{');
        int end = pat.find('}', beg);
        int padding = 0;
        if (beg != -1 && beg+1 != end) {
            std::string slen = pat.substr(beg+1, end-(beg+1));
            padding = std::stoi(slen);
        }

        // pre
        out << (beg==-1? pat : pat.substr(0,beg));
        // val
        long vbeg = out.tellp();
        out << a;
        if (padding) {
            int vlen = (long)out.tellp() - vbeg;
            int pad = padding - vlen;
            for (int i = 0; i < pad; ++i) {
                out << ' ';
            }
        }

        // post
        log(out, beg==-1? "" : pat.substr(end+1), args...);
    }


    template<typename T>
    static std::string str(T v) {
        std::stringstream ss;
        ss << v;
        return ss.str();
    }
    template<typename... ARGS>
    static std::string str(const std::string& pat, ARGS... args) {
        std::stringstream ss;
        log(ss, pat, args...);
        return ss.str();
    }


    static void log_head(std::ostream& out) {
        struct timeval tv{};
        gettimeofday(&tv, nullptr);
        struct tm* tm_info = localtime(&tv.tv_sec); // localtime(&tv.tv_sec);  std::gmtime(&time); auto time = std::time(nullptr);
        char strtime[40] = {};
        std::strftime(strtime, sizeof(strtime), "%F.%X", tm_info);

        //  << tm_info->tm_zone << ":"
        out << "[" << strtime <<"."<< tv.tv_usec << "]"
            << "["<<std::this_thread::get_id()<<"/INFO]: ";
    }

    template<typename... ARGS>
    static void info(const std::string& pat, ARGS... args)
    {
        std::stringstream ss;
        Log::log_head(ss);
        log(ss, pat, args...);
        std::cout << ss.str() << std::endl;
    }

    template<typename... ARGS>
    static void warn(const std::string& pat, ARGS... args)
    {
        std::stringstream ss;
        Log::log_head(ss);
        log(ss, pat, args...);
        std::cerr << ss.str() << std::endl;
    }


};

#endif //ETHERTIA_LOG_H
