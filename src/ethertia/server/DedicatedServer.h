//
// Created by Dreamtowards on 2022/12/23.
//

#ifndef ETHERTIA_DEDICATEDSERVER_H
#define ETHERTIA_DEDICATEDSERVER_H


#include <ethertia/network/Network.h>
#include <ethertia/network/packet/Packet.h>
#include <ethertia/server/ServerNetworkProcessor.h>

#include <ethertia/util/Log.h>
#include <ethertia/util/BenchmarkTimer.h>

class DedicatedServer
{
    inline static bool           m_Running = false;

    inline static int            m_ServerPort = 8081;
    inline static Network::Host* m_ServerHost = nullptr;

public:
    DedicatedServer() = delete;


    static void run()
    {
        startServer();

        while (m_Running)
        {
            runMainLoop();
        }

        stopServer();
    }

    static void startServer()
    {
        BenchmarkTimer _tm(nullptr, "Server initialized in {}.\n");
        m_Running = true;

        initConsoleThread();

        m_ServerHost = Network::newServer(m_ServerPort);
        Log::info("Server host listen on port {}", m_ServerPort);


        ServerNetworkProcessor::initPackets();

    }

    static void runMainLoop()
    {
        processNetwork();

        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }

    static void processNetwork()
    {

        Network::Polls(m_ServerHost, [](auto& e){  // Conn

            Log::info("New connection");
        }, [](ENetEvent& e) {  // Recv
            uint8_t* data = (uint8_t*)e.packet->data;
            size_t dataLen = e.packet->dataLength;

            Log::info("Received [{}]: '{}'", dataLen, std::string((char*)data, dataLen), Strings::hex(data, dataLen));

            Packet::ProcessPacket(data, dataLen);

        }, [](auto& e) {  // Drop

            Log::info("Disconn ", e.packet);
        });
    }


    static void stopServer()
    {


        Network::Deinit(m_ServerHost);
    }

    static void initConsoleThread()
    {
        new std::thread([]()
        {
            Log::info("Console thread is ready");

            while (DedicatedServer::isRunning())
            {
                std::string line;
                std::getline(std::cin, line);

                if (line[0] == '/')
                {

                }
                else
                {

                    Network::BroadcastPacket(m_ServerHost, (void*)line.c_str(), line.size());
                }
            }
        });
    }


    static const bool& isRunning() { return m_Running; }

};

#endif //ETHERTIA_DEDICATEDSERVER_H
