//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_NETWORKPROCESSOR_H
#define ETHERTIA_NETWORKPROCESSOR_H

#include <ethertia/network/Network.h>

#include <ethertia/network/packet/Packet.h>

#include <msgpack/msgpack.hpp>

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

                    Log::info("Client Connect");
                }, [](ENetEvent& e) {  // Recv

                    OnRecvPacket(e.packet->data, e.packet->dataLength);

                }, [](auto& e) {  // Drop
                    m_Connection = nullptr;
                });

                Timer::sleep_for(1);
            }


            Log::info("Network processor thread is shutdown due to disconnection.");

        });
    }

    static void OnRecvPacket(std::uint8_t* data, std::size_t len)
    {
        Log::info("Received {} bytes '{}'", len, std::string((char*)data, len));

        uint16_t PacketId = *data;  // first 2 bytes is PacketTypeId
        using PacketType = PacketChat; // GetType_by_PacketTypeId

        PacketType packet = msgpack::unpack<PacketType>(data + 2, len - 2);

        handlePacket(packet);

        Packet* p = CreatePacketInstance(PacketId);
        p.read(data);
//        eventbus.pust(p);
        p.dispatch();

        switch (PacketId) {
            case PacketChat::ID:
                handleChat(packet);
                break;
        }


    }

    static void handlePacket(PacketChat& packet) {

    }

    static void handleChat(PacketChat& packet) {



    }
};

#endif //ETHERTIA_NETWORKPROCESSOR_H
