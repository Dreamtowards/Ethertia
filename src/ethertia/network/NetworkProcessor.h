//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_NETWORKPROCESSOR_H
#define ETHERTIA_NETWORKPROCESSOR_H

#include <ethertia/network/Network.h>

#include <ethertia/network/packet/Packet.h>


// Client NetworkSystem

class NetworkProcessor
{
public:

    inline static Network::Host* m_NetworkHost = nullptr;
    inline static Network::Peer* m_Connection  = nullptr;


    static void init()
    {
        initPackets();

        m_NetworkHost = Network::newClient();
    }

    static void initPackets() {

        DEF_PACKET(1, PacketChat, NetworkProcessor::handlePacket);


    }

    static void deinit() {

        Network::Deinit(m_NetworkHost);
    }

    static void sendPacket(void* data, size_t len)
    {
        Log::info("PacketSent[{}]: '{}'\n{}", len, std::string((char*)data, len), Strings::hex(data, len));

        Network::SendPacket(m_Connection, data, len);
    }
    static void sendPacket(const std::string& str) {
        sendPacket((void*)str.c_str(), str.size());
    }

    template<typename PacketType>
    static void SendPacket(PacketType& packet)
    {
        std::vector<std::uint8_t> data = Packet::ComposePacket(packet);

        sendPacket(data.data(), data.size());
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

                    Log::info("Client Connect");
                }, [](ENetEvent& e) {  // Recv
                    Log::info("Received {} bytes '{}'", e.packet->dataLength, std::string((char*)e.packet->data, e.packet->dataLength));

                    Packet::ProcessPacket(e.packet->data, e.packet->dataLength);

                }, [](auto& e) {  // Drop
                    m_Connection = nullptr;
                });

                Timer::sleep_for(1);
            }


            Log::info("Network processor thread is shutdown due to disconnection.");

        });
    }


    static void handlePacket(const PacketChat& packet) {

        Log::info("HandleChat, content: ", packet.message);
    }
};

#endif //ETHERTIA_NETWORKPROCESSOR_H
