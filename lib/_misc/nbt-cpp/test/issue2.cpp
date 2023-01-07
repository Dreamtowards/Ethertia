#include <sstream>

#include "nbt.hpp"

#include "test.hpp"

using namespace nbt;

test {
    std::stringstream input(R"({ "voidByteArray": [B; ] })");
    tags::compound_tag root;
    input >> contexts::mojangson >> root;
    std::string key("voidByteArray");
    auto & bytes = root.get<std::vector<std::int8_t>>(key);
    assert_true(bytes.empty());
    std::cout << contexts::mojangson << root;
}
