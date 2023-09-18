//
// Created by Dreamtowards on 2022/4/22.
//

#pragma once

#include <string>
#include <iostream>
#include <mutex>
#include <format>
#include <chrono>
#include <sstream>  // for std::this_thread::get_id()
#include <source_location>


class Log
{
public:

    /// this function is global synchronous. so wouldn't cause 'Log Tearing' when multiple thread logging at the same time.
    /// @fmt if last char is '\1', then '\n' will not be automatically add at the end. (and the '\1' also wouldn't be print out.)
    ///      this is useful when u need append sth. e.g. BenchmarkTimer's time result.
    /// @lv  level text in the 'log_head' e.g. "DEBUG/INFO/WARN/ERROR"
    template<typename... Args>
    static void log(std::ostream& out, const char* _lv, const char* _loc, std::string_view fmt, Args&&... args)
    {
        static std::mutex g_LogLock;
        std::lock_guard<std::mutex> _guard(g_LogLock);  // prevents multiple logging in same time. have performance issue.

        bool keepline = fmt.back() == '\1';

        std::ostringstream ss;
        ss << std::this_thread::get_id();

        // log_head: e.g. "[2023-09-18 15:56:28.7610201][0x7ff850a73640/INFO]: "
        out << std::format("[{}][{}/{}]{}: ", 
            std::chrono::system_clock::now(), ss.str(), _lv, _loc);

        out << std::vformat(keepline ? fmt.substr(0, fmt.length() - 1) : fmt,
            std::make_format_args(std::forward<Args>(args)...));

        if (!keepline) {
            out << std::endl;
        }
    }


    template<typename... Args>
    static void info(std::string_view fmt, Args&&... args)
    {
        Log::log(std::cout, "INFO", "", fmt, std::forward<Args>(args)...);
    }

    template<typename... Args>
    static void warn(std::string_view fmt, Args&&... args)
    {
        Log::log(std::cout, "WARN", "", fmt, std::forward<Args>(args)...);
    }


};
