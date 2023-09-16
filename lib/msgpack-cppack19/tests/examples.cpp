//
// Created by Mike Loomis on 6/28/2019.
//

#include <catch2/catch.hpp>

#include "msgpack/msgpack.hpp"

struct Example {
  std::map<std::string, bool> map;

  template<class T>
  void pack(T &pack) {
    pack(map);
  }
};

TEST_CASE("Website example") {
  Example example{{{"compact", true}, {"schema", false}}};
  auto data = msgpack::pack(example);

  REQUIRE(data.size() == 18);
  REQUIRE(data == std::vector<uint8_t>{0x82, 0xa7, 0x63, 0x6f, 0x6d, 0x70, 0x61, 0x63, 0x74, 0xc3, 0xa6, 0x73, 0x63,
                                       0x68, 0x65, 0x6d, 0x61, 0xc2});

  REQUIRE(example.map == msgpack::unpack<Example>(data).map);
}

TEST_CASE("Unpack with error handling") {
  Example example{{{"compact", true}, {"schema", false}}};
  auto data = std::vector<uint8_t>{0x82, 0xa7, 0x63, 0x6f, 0x6d, 0x70, 0x61, 0x63, 0x74, 0xc3, 0xa6, 0x73, 0x63};
  std::error_code ec{};
  auto unpacked_object = msgpack::unpack<Example>(data, ec);
  if (ec && ec == msgpack::UnpackerError::OutOfRange)
    REQUIRE(true);
  else
    REQUIRE(false);
}