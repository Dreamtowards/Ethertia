//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_SERVERCONNECTIONPROC_H
#define ETHERTIA_SERVERCONNECTIONPROC_H

#include <ethertia/network/DedicatedServer.h>
#include <ethertia/network/packet/Packet.h>
#include <ethertia/network/server/ServerConnection.h>

#include <ethertia/network/packet/PacketChat.h>


class ServerConnectionProc
{
public:

#define DECL_PACKET_PROC_ARGS ServerConnection* conn = (ServerConnection*)packet.tag;

    static void initPackets() {

        _InitPacketIds();

        INIT_PACKET_PROC(PacketChat, ServerConnectionProc::processPacket);
        INIT_PACKET_PROC(CPacketLogin, ServerConnectionProc::processLogin);

    }

    static void processPacket(const PacketChat& packet)
    {
        DECL_PACKET_PROC_ARGS;

        BroadcastPacket(PacketChat{
            Strings::fmt("<{}>: {}", conn->mPlayerName, packet.message)
        });
    }

    static void processLogin(const CPacketLogin& packet)
    {
        DECL_PACKET_PROC_ARGS;

        conn->mPlayerName = packet.uuid;
    }

    template<typename PacketType>
    static void BroadcastPacket(PacketType packet)
    {
        std::vector<std::uint8_t> data = Packet::ComposePacket(packet);

        Network::BroadcastPacket(DedicatedServer::m_ServerHost, data.data(), data.size());
    }

};

#endif //ETHERTIA_SERVERCONNECTIONPROC_H
