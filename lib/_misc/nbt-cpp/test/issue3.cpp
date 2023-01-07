#include <sstream>

#include "nbt.hpp"

#include "test.hpp"

using namespace nbt;

test {
    const std::string subject = R"({"list":[[B;54b]]})";

    std::stringstream input(subject);
    tags::compound_tag root;
    input >> contexts::mojangson >> root;
    const std::string key("list");
    auto & list = dynamic_cast<const tags::bytearray_list_tag &>(*root.value[key]).value;
    assert_equals(1u, list.size());
    const auto & bytes = list.front();
    assert_equals(1u, bytes.size());
    assert_equals(54, bytes.front());

    std::stringstream output;
    output << contexts::mojangson << root;
    assert_equals(subject, output.str());
}
