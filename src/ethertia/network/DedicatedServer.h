//
// Created by Dreamtowards on 2022/12/23.
//

#ifndef ETHERTIA_DEDICATEDSERVER_H
#define ETHERTIA_DEDICATEDSERVER_H


#include <ethertia/util/Log.h>
#include <ethertia/util/BenchmarkTimer.h>

#include <ethertia/network/Network.h>
#include <ethertia/network/packet/Packet.h>

class DedicatedServer
{
    inline static bool           m_Running = false;

    inline static int            m_ServerPort = 8081;

public:
    DedicatedServer() = delete;
    inline static Network::Host* m_ServerHost = nullptr;


    static void run()
    {
        startServer();

        while (m_Running)
        {
            runMainLoop();
        }

        stopServer();
    }

    static void startServer();

    static void runMainLoop();
    static void processNetwork();

    static void stopServer();


    static void initConsoleThread();

    static const bool& isRunning() { return m_Running; }



};

#endif //ETHERTIA_DEDICATEDSERVER_H
