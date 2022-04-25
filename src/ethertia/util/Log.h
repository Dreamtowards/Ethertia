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

class Log
{
public:

    static void info(const std::string& s)
    {
        char strtime[90];
        struct timeval tv;
        gettimeofday(&tv, nullptr);
        struct tm* tm_info = localtime(&tv.tv_sec); // std::gmtime(&time); auto time = std::time(nullptr);
        std::strftime(strtime, sizeof(strtime), "%F.%X", tm_info);


        std::cout << "[" << tm_info->tm_zone << "/" << strtime <<"."<< tv.tv_usec << "]["<<std::this_thread::get_id()<<"/INFO]: " << s << std::endl;
    }


};

#endif //ETHERTIA_LOG_H
