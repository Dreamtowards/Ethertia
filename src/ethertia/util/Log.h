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

//    template<typename T>
//    static std::string str(T v) {
//        std::stringstream ss;
//        ss << v;
//        return ss.str();
//    }

    static void log_head(std::ostream& out) {
        struct timeval tv{};
        gettimeofday(&tv, nullptr);
        struct tm* tm_info = localtime(&tv.tv_sec); // localtime(&tv.tv_sec);  std::gmtime(&time); auto time = std::time(nullptr);
        char strtime[40] = {};
        std::strftime(strtime, sizeof(strtime), "%F.%X", tm_info);

        out << "[" << tm_info->tm_zone << ":" << strtime <<"."<< tv.tv_usec << "]"
            << "["<<std::this_thread::get_id()<<"/INFO]: ";
    }

    static void log(std::ostream& out, const std::string& pat) {
        out << pat << std::endl;
    }

    template<typename A>
    static void log(std::ostream& out, const std::string& pat, A a) {
        int i = pat.find("{}");
        out << (i==-1? pat : pat.substr(0,i)) << a << (i==-1? "" : pat.substr(i+2)) << std::endl;
    }

    template<typename A, typename... ARGS>
    static void log(std::ostream& out, const std::string& pat, A a, ARGS... args) {
        int i = pat.find("{}");
        out << (i==-1? pat : pat.substr(0,i)) << a;
        log(out, i==-1? "" : pat.substr(i+2), args...);
    }

    template<typename... ARGS>
    static void info(const std::string& pat, ARGS... args)
    {
        Log::log_head(std::cout);
        log(std::cout, pat, args...);
    }

    template<typename... ARGS>
    static void warn(const std::string& pat, ARGS... args)
    {
        Log::log_head(std::cerr);
        log(std::cerr, pat, args...);
    }


};

#endif //ETHERTIA_LOG_H
