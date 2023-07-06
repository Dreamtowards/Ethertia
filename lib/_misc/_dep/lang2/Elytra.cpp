
#include <iostream>
#include <fstream>


char* read_file(const std::string& path)
{
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: ");
    }
    size_t filesize = (size_t)file.tellg();
    char* data = new char[filesize + 1];  // +1: add an extra '\0' at the end. for c_string format compatible.
    data[filesize] = '\0';  // set last extra byte to '\0'.

    file.seekg(0);
    file.read(data, filesize);
    file.close();

    return data;
}

int main()
{
    char* s = read_file("/Users/dreamtowards/Documents/YouRepository/Ethertia/src/ethertia/lang/test.et");

    std::cout << s << "\n";

    delete s;
    return 0;
}