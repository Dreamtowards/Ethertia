//
// Created by Dreamtowards on 2023/7/6.
//

#include "Log.h"

#include <thread>
#include <cmath>
#include <chrono>



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





//inline static std::mutex g_LockLog;
//
//
//template<typename... ARGS>
//void Log::_log(std::ostream& s, const char* _lv, const char* _loc, const std::string& pat, ARGS... args)
//{
//    std::lock_guard<std::mutex> _guard(g_LockLog);  // prevents multiple logging in same time. have performance issue.
//
//    bool keepline = pat[pat.length() - 1] == '\1';
//    std::stringstream ss;
//
//    Log::log_head(ss, _lv, _loc);
//    Strings::_fmt(ss, (keepline ? pat.substr(0, pat.length()-1) : pat), args...);
//
//    s << ss.str();
//
//    if (!keepline) {
//        s << std::endl;
//    }
//}

//template<typename... ARGS>
//void Log::info(const std::string& pat, ARGS... args)
//{
//    Log::_log(std::cout, "INFO", "", pat, args...);
//}
//
//
//template<typename... ARGS>
//void Log::warn(const std::string& pat, ARGS... args)
//{
//    Log::_log(std::cerr, "WARN", "", pat, args...);
//}








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


// +__FILE_NAME__+":" __LINE__ "/" __func__ "]"  Strings::fmt("[{}:{}#{}]", __FILE_NAME__, __LINE__, __func__)._cstr()
//#define GetSourceLoc() Strings::fmt("[{}:{}@{}]", __FILE_NAME__, __LINE__, __func__).c_str()
//#define Log(x) Log::_log(std::cout, "INFO", GetSourceLoc(), x)

//#define ASSERT_WARN(expr, msg) if (!(expr)) Log::warn(msg);
