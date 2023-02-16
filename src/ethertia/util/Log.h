//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_LOG_H
#define ETHERTIA_LOG_H

#include <string>
#include <iostream>
#include <thread>
#include <sstream>
#include <cmath>
#include <chrono>

#include <glm/vec3.hpp>

#include <ethertia/util/Strings.h>

// +__FILE_NAME__+":" __LINE__ "/" __func__ "]"  Strings::fmt("[{}:{}#{}]", __FILE_NAME__, __LINE__, __func__)._cstr()
//#define GetSourceLoc() Strings::fmt("[{}:{}@{}]", __FILE_NAME__, __LINE__, __func__).c_str()
//#define Log(x) Log::_log(std::cout, "INFO", GetSourceLoc(), x)


//template<typename T>
//static std::string lstr(T v) {
//    std::stringstream ss;
//    ss << v;
//    return ss.str();
//}

#define ASSERT_WARN(expr, msg) if (!(expr)) Log::warn(msg);

class Log
{
public:

//    template<typename OUT>
//    static void log(OUT& out, const std::string& pat) {
//        out << pat;
//    }
//
//    template<typename OUT, typename A, typename... ARGS>
//    static void log(OUT& out, const std::string& pat, A a, ARGS... args) {
//        int beg = pat.find('{');
//        int end = pat.find('}', beg);
//        int padding = 0;
//        if (beg != -1 && beg+1 != end) {
//            std::string slen = pat.substr(beg+1, end-(beg+1));
//            padding = std::stoi(slen);
//        }
//
//        // pre
//        out << (beg==-1? pat : pat.substr(0,beg));
//        // val
//        long vbeg = out.tellp();
//        out << a;
//        if (padding) {
//            int vlen = (long)out.tellp() - vbeg;
//            int pad = padding - vlen;
//            for (int i = 0; i < pad; ++i) {
//                out << ' ';
//            }
//        }
//
//        // post
//        log(out, beg==-1? "" : pat.substr(end+1), args...);
//    }
//
//
//    template<typename T>
//    static std::string str(T v) {
//        std::stringstream ss;
//        ss << v;
//        return ss.str();
//    }
//    template<typename... ARGS>
//    static std::string str(const std::string& pat, ARGS... args) {
//        std::stringstream ss;
//        log(ss, pat, args...);
//        return ss.str();
//    }

/*
Profile(
  .url =
);
 */


    static void log_head(std::ostream& out, const char* _lv = "INFO", const char* _loc = "") {

        double sec_micro = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        double sec_milli = sec_micro * 0.001;_loc="";

        Strings::_fmt(out, "[{}.{7}][{}/{}]{}: ",
                      Strings::time_fmt("%Y-%m-%d.%H:%M:%S"),
                      std::fmod(sec_milli, 1000.0f),
                      std::this_thread::get_id(), _lv,
                      _loc
        );
//        out << "[" << std::put_time(tm_info, "%Y-%m-%d.%H:%M:%S") << "." << (std::fmod(sec, 1000.0f)) << "]"
//            << "["<<std::this_thread::get_id()<<"/INFO]: ";

//        struct timeval tv{};
//        gettimeofday(&tv, nullptr);
//        struct tm* tm_info = localtime(&tv.tv_sec); // localtime(&tv.tv_sec);  std::gmtime(&time); auto time = std::time(nullptr);
//        char strtime[40] = {};
//        std::strftime(strtime, sizeof(strtime), "%F.%X", tm_info);
//
//        //  << tm_info->tm_zone << ":"
//        out << "[" << strtime <<"."<< tv.tv_usec << "]"
//            << "["<<std::this_thread::get_id()<<"/INFO]: ";
    }

    template<typename... ARGS>
    static void _log(std::ostream& s, const char* _lv, const char* _loc, const std::string& pat, ARGS... args)
    {
        bool keepline = pat[pat.length() - 1] == '\1';
        std::stringstream ss;

        Log::log_head(ss, _lv, _loc);
        Strings::_fmt(ss, (keepline ? pat.substr(0, pat.length()-1) : pat), args...);

        s << ss.str();

        if (!keepline) {
            s << std::endl;
        }
    }

    template<typename... ARGS>
    static void info(const std::string& pat, ARGS... args)
    {
        Log::_log(std::cout, "INFO", "", pat, args...);
//        std::stringstream ss;
//        Log::log_head(ss);
//        Strings::_fmt(ss, pat, args...);
//        std::cout << ss.str() << std::endl;
    }

    template<typename... ARGS>
    static void warn(const std::string& pat, ARGS... args)
    {
        Log::_log(std::cerr, "WARN", "", pat, args...);
//        std::stringstream ss;
//        Log::log_head(ss);
//        Strings::_fmt(ss, pat, args...);
//        std::cerr << ss.str() << std::endl;
    }


};

#endif //ETHERTIA_LOG_H
