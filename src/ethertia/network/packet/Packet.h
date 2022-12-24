//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_PACKET_H
#define ETHERTIA_PACKET_H

#include <string>

#include "PacketChat.h"

#include <bit>


#define DEF_PACKET(PacketId, PacketType, PacketHandler) \
            assert(Packet::Id<PacketType>::val == 0); \
            Packet::Id<PacketType>::val = PacketId; \
            Packet::_HANDLER_MAP[PacketId] = [](std::uint8_t* data, std::size_t len) { \
                PacketHandler(msgpack::unpack<PacketType>(data, len)); \
            };

class Packet
{
public:

    template<typename T>
    struct Id {
    public:
        inline static int val = 0;
    };

    using PacketHandlerRaw = std::function<void(std::uint8_t* data, std::size_t len)>;

    inline static std::unordered_map<std::uint16_t, PacketHandlerRaw> _HANDLER_MAP;



    static const PacketHandlerRaw& FindHandler(std::uint16_t PacketId)
    {
        auto it = Packet::_HANDLER_MAP.find(PacketId);
        if (it == Packet::_HANDLER_MAP.end())
            throw std::runtime_error("unsupported packet id.");

        std::end
        return it->second;
    }


};

#endif //ETHERTIA_PACKET_H
