//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_CLIENTNETWORKSYSTEM_H
#define ETHERTIA_CLIENTNETWORKSYSTEM_H

#include <ethertia/network/Network.h>
#include <ethertia/network/packet/Packet.h>

// Client NetworkSystem

class ClientNetworkSystem
{
public:

    inline static Network::Host* m_NetworkHost = nullptr;
    inline static Network::Peer* m_Connection  = nullptr;


    static void init()
    {
        Network::Init();

        m_NetworkHost = Network::newClient();
    }

    static void deinit()
    {
        Network::Deinit(m_NetworkHost);
    }

    static void disconnect()
    {
        Network::disconnect(m_Connection);
        m_Connection = nullptr;
    }

    static void connect(const std::string& hostname, int port)
    {
        assert(m_Connection == nullptr);  // singleton connection

        new std::thread([hostname, port]()
        {
            Log::info("Connecting to server {}:{} ...", hostname, port);
            m_Connection = Network::connect(m_NetworkHost, hostname, port);

            while (m_Connection)
            {
                Network::Polls(m_NetworkHost,
                [hostname, port](auto& e) {  // Conn

                    Log::info("Connection established {}:{}.", hostname, port);

                    SendPacket(CPacketLogin{
                        "MyName",
                        "MyToken"
                    });
                },
                [](ENetEvent& e) {  // Recv
                    // Log::info("Received {} bytes '{}'", e.packet->dataLength, std::string((char*)e.packet->data, e.packet->dataLength));

                    Packet::ProcessPacket(e.packet->data, e.packet->dataLength);

                },
                [](auto& e) {  // Drop
                    m_Connection = nullptr;
                });

                Timer::sleep_for(1);
            }

            Log::info("Disconnected. connection thread is shutdown.");
        });
    }


    static void SendPacket(void* data, size_t len)
    {
        assert(m_Connection);
        // Log::info("PacketSent[{}]: '{}'\n{}", len, std::string((char*)data, len), Strings::hex(data, len));

        Network::SendPacket(m_Connection, data, len);
    }

    template<typename PacketType>
    static void SendPacket(PacketType packet)
    {
        std::vector<std::uint8_t> data = Packet::ComposePacket(packet);

        SendPacket(data.data(), data.size());
    }

};

#endif //ETHERTIA_CLIENTNETWORKSYSTEM_H
