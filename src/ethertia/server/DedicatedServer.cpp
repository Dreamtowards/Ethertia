//
// Created by Dreamtowards on 2022/12/2.
//

#ifndef ETHERTIA_DEDICATEDSERVER_CPP
#define ETHERTIA_DEDICATEDSERVER_CPP

#include <ethertia/server/DedicatedServer.h>

//#include "ethertia/network/packet/Packet.h"
//#include "unordered_map"
//#include "msgpack/include/msgpack/msgpack.hpp"

#include <ethertia/util/Endian.h>

struct PacketTest
{
    int32_t aInt = 0;
    std::string someStr;
    int8_t aLttl = 0;

    template<typename T>
    void pack(T& pack) {
        pack(aInt, someStr, aLttl);
    }
};

int main()
{
    DedicatedServer::run();

//    Packet p;


//    PacketTest p;
//    p.aInt = 9991;
//    p.someStr = "SthAbc123";
//    p.aLttl = 8;
//
//    std::vector<uint8_t> da = msgpack::pack(p);
//
//
//    PacketTest de = msgpack::unpack<PacketTest>(da);
//
//    Log::info("{} {} {}", de.aInt, de.someStr, (int)de.aLttl);


    return 0;
}

#endif //ETHERTIA_DEDICATEDSERVER_CPP
