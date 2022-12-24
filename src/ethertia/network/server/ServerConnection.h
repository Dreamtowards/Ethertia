//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_SERVERCONNECTION_H
#define ETHERTIA_SERVERCONNECTION_H

#include <ethertia/network/Network.h>

/**
 * Server side Connection, to a Player/Client.
 */

class ServerConnection
{
public:
    Network::Peer* m_Peer = nullptr;

    std::string mPlayerName;

    ServerConnection(Network::Peer* _peer) : m_Peer(_peer) {}





};

#endif //ETHERTIA_SERVERCONNECTION_H
