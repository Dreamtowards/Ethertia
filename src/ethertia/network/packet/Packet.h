//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_PACKET_H
#define ETHERTIA_PACKET_H

#include <string>

#include "PacketChat.h"


class Packet
{
public:

    template<typename T>
    struct Id {
        static int val;
    };


    void write(std::ostream& buf) {

        uint32_t i = 3;
        std::string s = "PacketString123End";
        uint8_t j = 9;

        buf << i;
        buf << s;
        buf << j;
    }

    void read(std::istream& buf) {

        uint32_t i = 0;
        std::string s;
        uint8_t j = 0;

        buf >> i;
        buf >> s;
        buf >> j;

        Log::info("Pack: {}, {}, {}", i, s, j);
    }


    template<typename T>
    static int getPacketId() {

        Log::info("Packname: {}, id: {}", typeid(T).name(), typeid(T).hash_code());
    }

    static void initPacketRegisters() {

    }

    /**
     * Packet Serialize
     */

//    template<typename T>
//    static void write(T* packet, void* dst) {
//
//    }
//
//    static int read(void* raw, void* dst) {
//
//    }


};

#endif //ETHERTIA_PACKET_H
