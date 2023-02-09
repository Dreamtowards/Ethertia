////
//// Created by Dreamtowards on 2022/12/24.
////
//
//#ifndef ETHERTIA_ENDIAN_H
//#define ETHERTIA_ENDIAN_H
//
//#include <algorithm>
//
//class Endian
//{
//public:
//
//    static bool is_big_endian() {
//        std::uint16_t i = 1;
//        return *(std::uint8_t*)&i == 0;
//    }
//
//    inline static void reverse(void* data, std::size_t len) {
//        std::uint8_t* ptr = (std::uint8_t*)data;
//        std::reverse(ptr, ptr+len);
//    }
//
//    static std::uint32_t bigendian(std::uint32_t val) {
//        if (is_big_endian())
//            return val;
//        Endian::reverse(&val, 4);
//        return val;
//    }
//    static std::uint16_t bigendian(std::uint16_t val) {
//        if (is_big_endian())
//            return val;
//        Endian::reverse(&val, 2);
//        return val;
//    }
//
//    // Duplicated behavior
//    static std::uint16_t of_bigendian(std::uint16_t val) {
//        if (is_big_endian())
//            return val;
//        Endian::reverse(&val, 2);
//        return val;
//    }
//
////    static void _test() {
////
////        Log::info("IsBig: ", Endian::is_big_endian());
////
////        {
////            uint32_t i = 10;
////            int big = Endian::bigendian(i);
////            Log::info("Hex: ", Strings::hex(&i, 4));
////            Log::info("Big: ", Strings::hex(&big, 4));
////        }
////
////        {
////            uint16_t i = 8;
////            int big = Endian::bigendian(i);
////            Log::info("Hex: ", Strings::hex(&i, 2));
////            Log::info("Big: ", Strings::hex(&big, 2));
////        }
////    }
//
//};
//
//#endif //ETHERTIA_ENDIAN_H
