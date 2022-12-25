//
// Created by Dreamtowards on 2022/12/2.
//

#ifndef ETHERTIA_DEDICATEDSERVER_CPP
#define ETHERTIA_DEDICATEDSERVER_CPP


#include <ethertia/server/DedicatedServer.h>
#include <ethertia/network/server/ServerConnectionProc.h>



int main()
{
    DedicatedServer::run();


    return 0;
}


void DedicatedServer::startServer()
{
    BenchmarkTimer _tm(nullptr, "Server initialized in {}.\n");
    m_Running = true;

    initConsoleThread();

    ServerConnectionProc::initPackets();

    Network::Init();
    m_ServerHost = Network::newServer(m_ServerPort);
    Log::info("Server host listen on port {}", m_ServerPort);



}

void DedicatedServer::runMainLoop()
{
    processNetwork();

    std::this_thread::sleep_for(std::chrono::milliseconds(1));
}

void DedicatedServer::processNetwork()
{
    Network::Polls(m_ServerHost, [](ENetEvent& e){  // Conn

        Log::info("New connection");

        e.peer->data = new ServerConnection(e.peer);
    }, [](ENetEvent& e) {  // Recv
        uint8_t* data = (uint8_t*)e.packet->data;
        size_t dataLen = e.packet->dataLength;

        // Log::info("Received [{}]: '{}'\n{}", dataLen, std::string((char*)data, dataLen), Strings::hex(data, dataLen));

        Packet::ProcessPacket(data, dataLen, e.peer->data);

    }, [](ENetEvent& e) {  // Drop
        Log::info("Disconn ", e.packet);

        delete (ServerConnection*)e.peer->data;
    });
}

void DedicatedServer::stopServer()
{

    Network::Deinit(m_ServerHost);
}






void DedicatedServer::initConsoleThread()
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



#endif //ETHERTIA_DEDICATEDSERVER_CPP
