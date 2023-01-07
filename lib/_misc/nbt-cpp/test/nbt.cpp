#include <fstream>
#include <iostream>
#include <sstream>

#include "nbt.hpp"

#include "test.hpp"

std::string read_content(const std::string & filename) {
	std::ifstream file(filename);
	return std::string((std::istreambuf_iterator<char>(file)),std::istreambuf_iterator<char>());
}

template <std::size_t header_size>
void test_for(const std::string & filename, const nbt::context & ctxt, const std::string & tagname) {
	using namespace nbt;
	std::string content = read_content(filename);
	std::stringstream input(content);
	std::array<char, header_size> header;
	input.read(header.data(), header_size);
	tags::compound_tag root;
	input >> ctxt >> root;
	assert_equals(1u, root.value.size());
	//const auto & values = root.get<std::map<std::string, std::unique_ptr<tags::tag>>>(tagname);
	//for (const auto & each : values)
	//	std::cerr << each.first << ' ' << each.second->id() << std::endl;
	std::stringstream buffer;
	//output.write(header.data(), header_size);
	//output << ctxt << root;
	std::cout << contexts::mojangson << root << std::endl;
	buffer << contexts::mojangson << root;
	tags::compound_tag new_root(true);
	try {
		buffer >> contexts::mojangson >> new_root;
	} catch (...) {
		std::cerr << "POSITION: " << buffer.tellg() << std::endl;
		throw;
	}
	std::cout << contexts::mojangson << new_root;
	//assert_equals(content, result);
}

test {
	using namespace nbt;
	using namespace std::string_literals;
	test_for<0>("bigtest.nbt", contexts::java, "Level");
	test_for<8>("bedrock_level.dat", contexts::bedrock_disk, "");
}
