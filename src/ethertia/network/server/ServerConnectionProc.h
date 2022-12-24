//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_SERVERCONNECTIONPROC_H
#define ETHERTIA_SERVERCONNECTIONPROC_H

#include <ethertia/server/DedicatedServer.h>
#include <ethertia/network/packet/Packet.h>
#include <ethertia/network/server/ServerConnection.h>

#include <ethertia/network/packet/PacketChat.h>


class ServerConnectionProc
{
public:

    static void initPackets() {

        INIT_PACKET(1, PacketChat, ServerConnectionProc::processPacket);


    }

    static void processPacket(const PacketChat& packet)
    {
        ServerConnection* conn = (ServerConnection*)packet.tag;

        BroadcastPacket(PacketChat{
            Strings::fmt("From Server: <{}>: {}", conn->mPlayerName, packet.message)
        });
    }

    template<typename PacketType>
    static void BroadcastPacket(PacketType packet)
    {
        std::vector<std::uint8_t> data = Packet::ComposePacket(packet);

        Network::BroadcastPacket(DedicatedServer::m_ServerHost, data.data(), data.size());
    }

};

#endif //ETHERTIA_SERVERCONNECTIONPROC_H
