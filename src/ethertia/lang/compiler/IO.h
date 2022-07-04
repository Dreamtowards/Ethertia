//
// Created by Dreamtowards on 2022/6/5.
//

#ifndef ETHERTIA_IO_H
#define ETHERTIA_IO_H

class IO
{
public:

    static constexpr u8 digits[] = {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    static void hex(std::stringstream& ss, u8 v) {
        ss << digits[v >> 4] << digits[v & 0xF];
    }

    static std::string& uppercase(std::string& s, char till = '\0') {
        for (auto& c : s) {
            if (c == till) break;
            c = (char)toupper(c);
        }
        return s;
    }

    static std::string dump(u8* p, size_t len) {
        std::stringstream ss;
        for (int i = 0; i < len; ++i) {
            IO::hex(ss, p[i]);
            ss << ' ';
        }
        return ss.str();
    }

    // Memory Address Arrange is Platform-Dependent, dont use (u64*)p = ...; to assign, it cause PlatDep/UB

    static void st_16(u8* p, u16 v) {
        p[0] = v >> 8;
        p[1] = v;
    }
    static u16 ld_16(const u8* p) {
        return (p[0] << 8) | p[1];
    }
    static void st_32(u8* p, u32 v) {
        p[0] = v >> 24;
        p[1] = v >> 16;
        p[2] = v >> 8;
        p[3] = v;
    }
    static u32 ld_32(const u8* p) {
        return (p[0] << 24) | (p[1] << 16) | (p[2] << 8) | p[3];
    }

};

#endif //ETHERTIA_IO_H
