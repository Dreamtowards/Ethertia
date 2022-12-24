//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_PACKETCHAT_H
#define ETHERTIA_PACKETCHAT_H

#include "Packet.h"

struct PacketChat
{
    std::string message;


    DECL_PACKET(message);

};

struct CPacketLogin
{
    std::string uuid;
    std::string token;

    DECL_PACKET(uuid, token);
};

#endif //ETHERTIA_PACKETCHAT_H
