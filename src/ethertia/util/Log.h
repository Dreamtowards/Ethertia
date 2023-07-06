//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_LOG_H
#define ETHERTIA_LOG_H

#include <string>
#include <iostream>
#include <sstream>

#include <ethertia/util/Strings.h>


class Log
{
public:


    /// produce a "log_head" e.g. "[2023-07-06.13:44:03.623.765][0x7ff850a73640/INFO]: "
    static void log_head(std::ostream& out, const char* _lv = "INFO", const char* _loc = "");

    /// this function is global Synchronous. so wouldn't cause 'Log Tearing' when multiple thread logging at the same time.
    /// @pat if last char is '\1', then '\n' will not be automatically add at the end. (and the '\1' also wouldn't be print out.)
    ///      this is useful when u need append sth. e.g. BenchmarkTimer's time result.
    /// @lv  level text in the 'log_head' e.g. "DEBUG/INFO/WARN/ERROR"
    template<typename... ARGS>
    static void _log(std::ostream& s, const char* _lv, const char* _loc, const std::string& pat, ARGS... args)
    {
        static std::mutex g_LogLock;
        std::lock_guard<std::mutex> _guard(g_LogLock);  // prevents multiple logging in same time. have performance issue.

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
    }

    template<typename... ARGS>
    static void warn(const std::string& pat, ARGS... args)
    {
        Log::_log(std::cout, "WARN", "", pat, args...);
    }


};

#endif //ETHERTIA_LOG_H
