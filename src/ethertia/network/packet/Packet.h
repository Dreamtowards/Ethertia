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



#define DECL_PACKET(X, ...)   \
    void* tag = nullptr; \
    template<typename T> \
    void pack(T& pack) { \
        pack(X, ##__VA_ARGS__); \
    }

#define INIT_PACKET_ID(PacketId, PacketType)  \
    assert(Packet::Id<PacketType>::val == 0); \
    Packet::Id<PacketType>::val = PacketId;

#define INIT_PACKET_PROC(PacketType, PacketHandler) \
    assert(Packet::Id<PacketType>::val != 0); \
    Packet::_HANDLER_MAP[Packet::Id<PacketType>::val] = [](std::uint8_t* data, std::size_t len, void* tag) { \
        PacketType p = msgpack::unpack<PacketType>(data, len); \
        p.tag = tag; \
        PacketHandler(p); \
    };


class Packet
{
public:
    using PacketId_t = std::uint16_t;

    template<typename T>
    struct Id {
    public:
        inline static PacketId_t val = 0;
    };

    using PacketHandler = std::function<void(std::uint8_t* data, std::size_t len, void* tag)>;

    inline static std::unordered_map<PacketId_t, Packet::PacketHandler> _HANDLER_MAP;



    static const PacketHandler& FindHandler(PacketId_t PacketId)
    {
        auto it = Packet::_HANDLER_MAP.find(PacketId);
        if (it == Packet::_HANDLER_MAP.end())
            throw std::runtime_error("no handler for packet id " + std::to_string(PacketId));

        return it->second;
    }

    static void ProcessPacket(std::uint8_t* data, std::size_t len, void* tag = nullptr)
    {
        PacketId_t PacketId = Endian::of_bigendian(*(PacketId_t*)data);  // first 2 bytes is PacketTypeId

        auto& handler = Packet::FindHandler(PacketId);

        handler(data + 2, len - 2, tag);
    }


    template<typename PacketType>
    static std::vector<std::uint8_t> ComposePacket(PacketType& packet)
    {
        std::vector<std::uint8_t> data = msgpack::pack(packet);

        // Write PacketId

        PacketId_t PacketId = Packet::Id<PacketType>::val;
        assert(PacketId != 0);

        data.insert(data.begin(), 2, 0);

        uint16_t be = Endian::bigendian(PacketId);
        Collections::cpy(&be, data.data(), 2);

        return data;
    }

};

#endif //ETHERTIA_PACKET_H
