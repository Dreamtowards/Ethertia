NBT library for C++17
=======================================

You can read and save files in NBT format with this library. It supports different formats:
NBT Java Edition, NBT Bedrock Edition (both network and file formats) and Mojangson.

Usage
---------------------------------------

```c++
#include <nbt.hpp>
#include <fstream>

using namespace nbt;

int main() {
    std::ifstream input("java.nbt");
    tags::compound_tag root;
    input >> contexts::java >> root;
    std::ofstream output("mojangson.txt");
    output << contexts::mojangson << root;
}
```

By default all lists have their own specialization for each tag. If you want to get something simple
like `std::vector<nbt::tags::tag>` as list values you can use `nbt::tags::tag_compound::make_heavy`
and `nbt::tags::list_tag::as_tags`. After that all lists in NBT subtree would be
`nbt::tags::tag_list_tag` objects.

NBTC
---------------------------------------

There are also a simple converter between NBT formats: `nbtc`.

    ./nbtc FROM TO

### Parameters:
- FROM          read stdin as FROM NBT format
- TO            write NBT tag to stdout as TO format

### NBT formats:
- *java* Minecraft Java Edition NBT
- *bedrock* or *bedrock-disk* Minecraft Bedrock Edition storage NBT format
- *bedrock-net* Minecraft Bedrock Edition network NBT format
- *mojangson* text NBT representation
- *kbt* HandTruth NBT extension
