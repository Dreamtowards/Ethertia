[![Build status](https://ci.appveyor.com/api/projects/status/h9avws048watkvnr?svg=true)](https://ci.appveyor.com/project/gregjesl/simpleson)

# simpleson
Lightweight C++ JSON parser &amp; serializer that is C++98 compatible with no dependencies

[Github Repository](https://github.com/gregjesl/simpleson)

[Documentation](https://www.oldgreg.net/simpleson/1.1.0)

## Why simpleson? 
Simpleson is built under the following requirements:
- One header and one source file only
- No external dependencies
- ISO/IEC 14882:1998 (aka C++98) compatible
- Cross-platform

A primary use case for simpleson is in an memory-constrained embedded system.  

## Building simpleson
Simpleson was intentionally built such that a developer could simply copy [json.h](json.h) into the target project's `inc` folder, copy [json.cpp](json.cpp) into the `src` folder, and then compile the target project.  No linking -> no drama.  

Building the library and tests follows the standard build chain via CMake:
```
mkdir build
cd build
cmake ../
make
```
Unit tests can then be run by executing `make test`

## Quickstart

```cpp
// Create the input
std::string input = "{ \"hello\": \"world\" }";

// Parse the input
json::jobject result = json::jobject::parse(input);

// Get a value
std::string value = (std::string)result.get_entry("hello").value

// Add entries
json::jobject example;
example["int"] = 123;
example["float"] = 12.3f;
example["string"] = "test string";
int test_array[3] = { 1, 2, 3 };
example["array"] = std::vector<int>(test_array, test_array + 3);
std::string test_string_array[2] = { "hello", "world" };
example["strarray"] = std::vector<std::string>(test_string_array, test_string_array + 2);
example["emptyarray"] = std::vector<std::string>();
example["boolean"].set_boolean(true);
example["null"].set_null();

// Reference values
std::string hello = example["strarray"][0];
std::string world = example["strarray"][1];

// Clear a value
example["hello"].clear();
example.remove("emptyarray");

// Serialize the new object
std::string serial = (std::string)result;
```

## Using simpleson

The namespace of simpleson is simply `json`.  JSON objects can be parsed by calling `json::jobject::parse()`, which takes a string and returns a `jobject`. The array operators are overloaded for `jobject`, meaning you can assign and access entries using the `[]` operators like many other software languages. Other useful methods of `jobject` include:
- `has_key("key")` - Returns true if the key exists in the `jobject`
- `remove("key")` - Removes they entry associated with the key if the key exists in the `jobject`
- `clear()` - Removes all entries in the `jobject`
- `["key"].set_boolean(true)` - [Sets the key to the boolean value](#a-note-on-booleans)
- `["key"].set_null()` - Sets the value associated with the key to null
- `["key"].is_true()` - Returns true if the boolean value associated with the key is true
- `["key"].is_null()` - Returns true if the value associated with the key is null
- `pretty()` - Serializes the object into a "pretty" string (using tabs and newlines)

An instance of `jobject` can be searlized by casting it to a `std::string`.  Note that an instance of `jobject` does not retain it's original formatting (it drops tabs, spaces outside strings, and newlines).  

### Arrays
Simpleson supports arrays as the root object: 
```cpp
json::jobject example = json::jobject::parse("[1,2,3]");
int one = example[0]; // Value is 1
int two = example[1]; // Value is 2
int three = example[2]; // Value is 3
```
Arrays are stored in a `jobject`. You can determine whether a `jobject` is holding an array through the method `is_array()`. 

### Multi-level Access
To access elements several levels down, the `get(key)` and `array(index)` can be used for objects and arrays, respectively: 
```cpp
std::string music_desired = example.array(0).get("hobbies").array(1).get("music");
```
See [the full example here](examples/rootarray.cpp). 

### A note on booleans
Booleans are handled a bit differently than other data types. Since everything can be cast to a boolean, having an implicit boolean operator meant everything goes to a boolean! Instead, **boolean values are set by using the `set_boolean()` method**. If you do not use this method and instead directly create/assign a boolean to a `jobject` array entry, then the boolean will be cast to an int with a value of 0 or 1. Similarly, you can check if a value is set to true or false using the `is_true()` method. 
