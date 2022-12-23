//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_NETWORKPROCESSOR_H
#define ETHERTIA_NETWORKPROCESSOR_H

#include <ethertia/network/Network.h>

// Client NetworkSystem

class NetworkProcessor
{
public:

    inline static Network::Host* m_NetworkHost = nullptr;
    inline static Network::Peer* m_Connection  = nullptr;


    static void init() {

        m_NetworkHost = Network::newClient();
    }

    static void deinit() {

        Network::Deinit(m_NetworkHost);
    }

    static void sendPacket(const std::string& str) {
        sendPacket((void*)str.c_str(), str.size());
    }
    static void sendPacket(void* data, size_t len) {
        Network::SendPacket(m_Connection, data, len);
    }

    static void disconnect()
    {
        Network::disconnect(m_Connection);
        m_Connection = nullptr;
    }

    static void connect(const std::string& hostname, int port) {

        new std::thread([hostname, port]()
        {
            {
                BenchmarkTimer _tm;
                Log::info("Connecting to server {}:{} ...", hostname, port);
                m_Connection = Network::connect(m_NetworkHost, hostname, port);
                Log::info("Server connected.\1");
            }

            while (m_Connection)
            {
                Network::Polls(m_NetworkHost, [](auto& e){  // Conn



                }, [](auto& e) {  // Recv
                    void* data = e.packet->data;
                    size_t dataLen = e.packet->dataLength;

                    Log::info("Received {} bytes '{}'", dataLen, std::string((char*)data, dataLen));
                }, [](auto& e) {  // Drop



                    Log::info("Disconn ");

                    m_Connection = nullptr;
                });

                Timer::sleep_for(1);
            }


            Log::info("Network processor thread is shutdown due to disconnection.");

        });
    }



};

#endif //ETHERTIA_NETWORKPROCESSOR_H
