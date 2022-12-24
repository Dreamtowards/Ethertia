//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_PACKET_H
#define ETHERTIA_PACKET_H

#include <string>
#include <unordered_map>

#include <system_error>
#include <msgpack/msgpack.hpp>

#include <ethertia/util/Endian.h>
#include <ethertia/util/Collections.h>

#include "PacketChat.h"



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
            throw std::runtime_error("unsupported packet id " + std::to_string(PacketId));

        return it->second;
    }

    static void ProcessPacket(std::uint8_t* data, std::size_t len)
    {
        uint16_t PacketId = Endian::of_bigendian(*data);  // first 2 bytes is PacketTypeId

        auto& handler = Packet::FindHandler(PacketId);

        handler(data + 2, len - 2);
    }


    template<typename PacketType>
    static std::vector<std::uint8_t> ComposePacket(PacketType& packet)
    {
        std::vector<std::uint8_t> data = msgpack::pack(packet);

        // Write PacketId

        std::uint16_t PacketId = Packet::Id<PacketType>::val;
        assert(PacketId != 0);

        data.insert(data.begin(), 2, 0);

        uint16_t be = Endian::bigendian(PacketId);
        Collections::cpy(&be, data.data(), 2);

        return data;
    }

};

#endif //ETHERTIA_PACKET_H
