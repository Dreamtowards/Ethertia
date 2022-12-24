//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_PACKETCHAT_H
#define ETHERTIA_PACKETCHAT_H

#include <string>

struct PacketChat
{

    std::string message;
    int sthOther = 0;



//#define PACK(X) template<typename T> \
//                void pack(T& pack) { \
//                    pack(X); \
//                }                    \
//
//    PACK(message);

    template<typename T>
    void pack(T& pack) {
        pack(message, sthOther);
    }

};

#endif //ETHERTIA_PACKETCHAT_H
