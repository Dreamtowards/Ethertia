//
// Created by Mike Loomis on 6/22/2019.
//

#include <catch2/catch.hpp>

#include <msgpack/msgpack.hpp>

TEST_CASE("Nil type packing") {
  auto null_obj = std::nullptr_t{};
  auto packer = msgpack::Packer{};
  packer.process(null_obj);
  REQUIRE(packer.vector() == std::vector<uint8_t>{0xc0});
}

TEST_CASE("Boolean type packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};
  auto bool_obj = false;
  packer.process(bool_obj);
  unpacker.set_data(packer.vector().data(), packer.vector().size());
  bool_obj = true;
  unpacker.process(bool_obj);
  REQUIRE(packer.vector() == std::vector<uint8_t>{0xc2});
  REQUIRE(!bool_obj);

  bool_obj = true;
  packer.clear();
  packer.process(bool_obj);
  unpacker.set_data(packer.vector().data(), packer.vector().size());
  bool_obj = false;
  unpacker.process(bool_obj);
  REQUIRE(packer.vector() == std::vector<uint8_t>{0xc3});
  REQUIRE(bool_obj);
}

TEST_CASE("Integer type packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};

  for (auto i = 0U; i < 10; ++i) {
    uint8_t test_num = i * (std::numeric_limits<uint8_t>::max() / 10);
    packer.clear();
    packer.process(test_num);
    uint8_t x = 0U;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }

  for (auto i = 0U; i < 10; ++i) {
    uint16_t test_num = i * (std::numeric_limits<uint16_t>::max() / 10);
    packer.clear();
    packer.process(test_num);
    uint16_t x = 0U;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }

  for (auto i = 0U; i < 10; ++i) {
    uint32_t test_num = i * (std::numeric_limits<uint32_t>::max() / 10);
    packer.clear();
    packer.process(test_num);
    uint32_t x = 0U;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }

  for (auto i = 0U; i < 10; ++i) {
    uint64_t test_num = i * (std::numeric_limits<uint64_t>::max() / 10);
    packer.clear();
    packer.process(test_num);
    uint64_t x = 0U;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }

  for (auto i = -5; i < 5; ++i) {
    int8_t test_num = i * (std::numeric_limits<int8_t>::max() / 5);
    packer.clear();
    packer.process(test_num);
    int8_t x = 0;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }

  for (auto i = -5; i < 5; ++i) {
    int16_t test_num = i * (std::numeric_limits<int16_t>::max() / 5);
    packer.clear();
    packer.process(test_num);
    int16_t x = 0;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }

  for (auto i = -5; i < 5; ++i) {
    int32_t test_num = i * (std::numeric_limits<int32_t>::max() / 5);
    packer.clear();
    packer.process(test_num);
    int32_t x = 0;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }

  for (auto i = -5; i < 5; ++i) {
    int64_t test_num = i * (std::numeric_limits<int64_t>::max() / 5);
    packer.clear();
    packer.process(test_num);
    int64_t x = 0;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }
}

TEST_CASE("Chrono type packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};

  auto test_time_point = std::chrono::steady_clock::now();
  auto test_time_point_copy = test_time_point;

  packer.process(test_time_point);
  test_time_point = std::chrono::steady_clock::time_point{};
  REQUIRE(test_time_point != test_time_point_copy);
  unpacker.set_data(packer.vector().data(), packer.vector().size());
  unpacker.process(test_time_point);
  REQUIRE(test_time_point == test_time_point_copy);
}

TEST_CASE("Float type packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};

  for (auto i = -5; i < 5; ++i) {
    float test_num = 5.0f + float(i) * 12345.67f / 4.56f;
    packer.clear();
    packer.process(test_num);
    float x = 0.0f;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }

  for (auto i = -5; i < 5; ++i) {
    double test_num = 5.0 + double(i) * 12345.67 / 4.56;
    packer.clear();
    packer.process(test_num);
    double x = 0.0;
    unpacker.set_data(packer.vector().data(), packer.vector().size());
    unpacker.process(x);
    REQUIRE(x == test_num);
  }
}

TEST_CASE("String type packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};

  auto str1 = std::string("test");
  packer.process(str1);
  str1 = "";
  unpacker.set_data(packer.vector().data(), packer.vector().size());
  unpacker.process(str1);
  REQUIRE(packer.vector() == std::vector<uint8_t>{0b10100000 | 4, 't', 'e', 's', 't'});
  REQUIRE(str1 == "test");
}

TEST_CASE("Byte array type packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};

  auto vec1 = std::vector<uint8_t>{1, 2, 3, 4};
  packer.process(vec1);
  vec1.clear();
  unpacker.set_data(packer.vector().data(), packer.vector().size());
  unpacker.process(vec1);
  REQUIRE(packer.vector() == std::vector<uint8_t>{0xc4, 4, 1, 2, 3, 4});
  REQUIRE(vec1 == std::vector<uint8_t>{1, 2, 3, 4});
}

TEST_CASE("Array type packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};

  auto list1 = std::list<std::string>{"one", "two", "three"};
  packer.process(list1);
  list1.clear();
  unpacker.set_data(packer.vector().data(), packer.vector().size());
  unpacker.process(list1);
  REQUIRE(packer.vector() == std::vector<uint8_t>{0b10010000 | 3, 0b10100000 | 3, 'o', 'n', 'e',
                                                  0b10100000 | 3, 't', 'w', 'o',
                                                  0b10100000 | 5, 't', 'h', 'r', 'e', 'e'});
  REQUIRE(list1 == std::list<std::string>{"one", "two", "three"});
}

TEST_CASE("std::array type packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};

  auto arr = std::array<std::string, 3>{"one", "two", "three"};
  packer.process(arr);
  arr.fill("");
  unpacker.set_data(packer.vector().data(), packer.vector().size());
  unpacker.process(arr);
  REQUIRE(packer.vector() == std::vector<uint8_t>{0b10010000 | 3, 0b10100000 | 3, 'o', 'n', 'e',
                                                  0b10100000 | 3, 't', 'w', 'o',
                                                  0b10100000 | 5, 't', 'h', 'r', 'e', 'e'});
  REQUIRE(arr == std::array<std::string, 3>{"one", "two", "three"});
}

TEST_CASE("Map type packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};

  auto map1 = std::map<uint8_t, std::string>{std::make_pair(0, "zero"), std::make_pair(1, "one")};
  packer.process(map1);
  map1.clear();
  unpacker.set_data(packer.vector().data(), packer.vector().size());
  unpacker.process(map1);
  REQUIRE(packer.vector() == std::vector<uint8_t>{0b10000000 | 2, 0, 0b10100000 | 4, 'z', 'e', 'r', 'o',
                                                  1, 0b10100000 | 3, 'o', 'n', 'e'});
  REQUIRE(map1 == std::map<uint8_t, std::string>{std::make_pair(0, "zero"), std::make_pair(1, "one")});
}

TEST_CASE("Unordered map packing") {
  auto packer = msgpack::Packer{};
  auto unpacker = msgpack::Unpacker{};

  auto map1 = std::unordered_map<uint8_t, std::string>{std::make_pair(0, "zero"), std::make_pair(1, "one")};
  auto map_copy = map1;
  packer.process(map1);
  map1.clear();
  unpacker.set_data(packer.vector().data(), packer.vector().size());
  unpacker.process(map1);
  REQUIRE(map1[0] == map_copy[0]);
  REQUIRE(map1[1] == map_copy[1]);
}