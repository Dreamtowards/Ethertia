//
// Created by Dreamtowards on 2022/12/24.
//

#ifndef ETHERTIA_IO_H
#define ETHERTIA_IO_H


class IO
{
public:

    static bool is_big_endian() {
        std::uint16_t i = 1;
        return *(std::uint8_t*)&i == 0;
    }

};

#endif //ETHERTIA_IO_H
