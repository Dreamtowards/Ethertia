//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_SERVERNETWORKPROCESSOR_H
#define ETHERTIA_SERVERNETWORKPROCESSOR_H

#include <ethertia/network/packet/Packet.h>

class ServerNetworkProcessor
{
public:

    static void initPackets() {

        DEF_PACKET(1, PacketChat, ServerNetworkProcessor::processPacket);


    }

    static void processPacket(const PacketChat& packet)
    {

        Network::BroadcastPacket(DelicatedServer::)
    }

};

#endif //ETHERTIA_SERVERNETWORKPROCESSOR_H
