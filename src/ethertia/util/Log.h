//
// Created by Dreamtowards on 2022/4/22.
//

#ifndef ETHERTIA_LOG_H
#define ETHERTIA_LOG_H

#include <string>
#include <iostream>
#include <chrono>

class Log
{
public:

    static void info(const std::string& s)
    {
        char strtime[90];
        auto t = std::time(nullptr);
        std::strftime(strtime, sizeof(strtime), "[%F.%X]", std::gmtime(&t));

        std::cout << strtime << "[main/INFO]: " << s << std::endl;
    }


};

#endif //ETHERTIA_LOG_H
