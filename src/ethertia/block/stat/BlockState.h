//
// Created by Dreamtowards on 2022/8/23.
//

#ifndef ETHERTIA_BLOCKSTATE_H
#define ETHERTIA_BLOCKSTATE_H

// Load
// 1block = 32bit(4byte), 1chunk = 16^3 block = 4096*4byte = 16kb
// 5km = 312^3 chunks, = 474,552 mb
// 1km = 62^3 chunks,  = 3,728 mb

class BlockState
{
public:

    u8 id = 0;
    float density = 0;  // 0-255 / 255
    u8 metadata   = 0;

    BlockState(u8 id = 0, float density = 0.0f, u8 metadata = 0) : id(id), density(density), metadata(metadata) {}

};

#endif //ETHERTIA_BLOCKSTATE_H
