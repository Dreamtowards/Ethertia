//
// Created by Mike Loomis on 7/31/2019.
//

#include <catch2/catch.hpp>

#include <msgpack/msgpack.hpp>

struct NestedObject {
  std::string nested_value{};

  template<class T>
  void pack(T &pack) {
    pack(nested_value);
  }
};

struct BaseObject {
  int first_member{};
  NestedObject second_member{};

  template<class T>
  void pack(T &pack) {
    pack(first_member, second_member);
  }
};

TEST_CASE("Can serialize user objects") {
  auto object = BaseObject{12345, {"NestedObject"}};
  auto data = msgpack::pack(object);
  auto unpacked_object = msgpack::unpack<BaseObject>(data);

  REQUIRE(object.first_member == unpacked_object.first_member);
  REQUIRE(object.second_member.nested_value == unpacked_object.second_member.nested_value);
}