#include <iostream>
#include <sstream>
#include <algorithm>
#include <map>
#include <nbt.hpp>

const std::map<std::string, nbt::context> contexts_by_name {
	{ "java", nbt::contexts::java },
	{ "bedrock-net", nbt::contexts::bedrock_net },
	{ "bedrock", nbt::contexts::bedrock_disk },
	{ "bedrock-disk", nbt::contexts::bedrock_disk },
	{ "kbt", nbt::contexts::kbt },
	{ "mojangson", nbt::contexts::mojangson }
};

void usage(std::ostream & output, const char * program) {
	output << "Usage: " << program << R"===( <FROM> <TO>

Parameters:
  FROM          read stdin as FROM NBT format
  TO            write NBT tag to stdout as TO format
NBT formats:
  java          Minecraft Java Edition NBT
  bedrock       Minecraft Bedrock Edition storage NBT format
  bedrock-disk
  bedrock-net   Minecraft Bedrock Edition network NBT format
  mojangson     text NBT representation
  kbt           handtruth NBT extension
)===";
}

int main(int argc, char ** argv) {
	if (argc == 2 && std::string(argv[1]) == "--help") {
		usage(std::cout, argv[0]);
		return EXIT_SUCCESS;
	}
	if (argc != 3) {
		std::cerr << "exactly 2 arguments required" << std::endl;
		usage(std::cerr, argv[0]);
		return EXIT_FAILURE;
	}
	auto iter_from = contexts_by_name.find(argv[1]);
	auto iter_to = contexts_by_name.find(argv[2]);
	if (iter_from == contexts_by_name.end()) {
		std::cerr << "unknown FROM format: " << argv[1] << std::endl;
		usage(std::cerr, argv[0]);
		return EXIT_FAILURE;
	}
	if (iter_to == contexts_by_name.end()) {
		std::cerr << "unknown TO format: " << argv[2] << std::endl;
		usage(std::cerr, argv[0]);
		return EXIT_FAILURE;
	}
	using namespace nbt;
	const context & from = iter_from->second;
	const context & to = iter_to->second;
	std::unique_ptr<tags::tag> root;
	auto & input = std::cin;
	input >> from;
	std::cout << to;
	try {
		if (from.format == context::formats::mojangson) {
			tag_id id = deduce_tag(input);
			std::unique_ptr<tags::tag> tag = tags::read(id, input);
			if (tag->id() == tag_id::tag_compound) {
				root = std::move(tag);
			} else {
				auto compound = std::make_unique<tags::compound_tag>(true);
				compound->value[""] = std::move(tag);
				root = std::move(compound);
			}
		} else {
			root = tags::read<tag_id::tag_compound>(input);
		}
	} catch (const std::exception & e) {
		std::cerr
			<< "failed to read NBT (stream position "
			<< input.tellg() << "): " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	try {
		std::cout << *root;
		if (context::formats::mojangson == to.format)
			std::cout << std::endl;
	} catch (const std::exception & e) {
		std::cerr
			<< "failed to write NBT (stream position "
			<< std::cout.tellp() << "): " << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
