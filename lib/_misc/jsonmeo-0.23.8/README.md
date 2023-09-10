<div align="center">

# meojson

现代化的全平台 Json/Json5 解析/生成器，Header-only，并附带大量语法糖！

A modern all-platform Json/Json5 parser/serializer, which is header-only and contains lots of syntactic sugar!

</div>

[English](./README_en.md)

## 使用说明

- 在您的项目中包含头文件即可使用  

```cpp
#include "json.hpp"
```

- 若您需要解析 Json5, 则请包含 `json5.hpp` 头文件

```cpp
#include "json5.hpp"
```

- meojson 仅依赖 STL, 但需要 c++17 标准

## 示例

### 解析

```cpp
/***
 * from sample/sample.cpp
***/
#include <iostream>
#include "json.hpp"

void parsing()
{
    std::string content = R"(
{
    "repo": "meojson",
    "author": {
        "MistEO": "https://github.com/MistEO",
        "ChingCdesu": "https://github.com/ChingCdesu"
    },
    "list": [ 1, 2, 3 ],
    "str": "abc\n123",
    "num": 3.1416,
    "A_obj": {
        "B_arr": [
            { "C_str": "i am a distraction" },
            { "C_str": "you found me!" }
        ]
    }
}
    )";

    auto ret = json::parse(content);

    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return;
    }
    auto& value = ret.value(); // you can use rvalues if needed, like
                               // `auto value = std::move(ret).value();`
    // Output: meojson
    std::cout << value["repo"].as_string() << std::endl;

    /* Output:
        ChingCdesu's homepage: https://github.com/ChingCdesu
        MistEO's homepage: https://github.com/MistEO
    */
    for (auto&& [name, homepage] : value["author"].as_object()) {
        std::cout << name << "'s homepage: " << homepage.as_string() << std::endl;
    }

    // Output: abc
    std::string str = (std::string)value["str"];    // it is equivalent to `value["str"].as_string()`
    std::cout << str << std::endl;

    // Output: 3.141600
    double num = value["num"].as_double();          // similarly, you can use `(double)value["num"]`
    std::cout << num << std::endl;

    // Output: default_value
    std::string get = value.get("maybe_exists", "default_value");
    std::cout << get << std::endl;

    // Output: you found me!
    std::string nested_get = value.get("A_obj", "B_arr", 1, "C_str", "default_value");
    std::cout << nested_get << std::endl;

    // Output: 1, 2, 3
    // If the "list" is not an array or not exists, it will be a invalid optional;
    auto opt = value.find<json::array>("list");
    if (opt) {
        auto& arr = opt.value();
        for (auto&& elem : arr) {
            std::cout << elem.as_integer() << std::endl;
        }
    }
    // more examples, it will output 3.141600
    auto opt_n = value.find<double>("num");
    if (opt_n) {
        std::cout << opt_n.value() << std::endl;
    }
    // If you use the `find` without template argument, it will return a `std::optional<json::value>`
    auto opt_v = value.find("not_exists");
    std::cout << "Did we find the \"not_exists\"? " << opt_v.has_value() << std::endl;

    // Output: "literals"
    using namespace json::literals;
    auto val = "{\"hi\":\"literals\"}"_json;
    std::cout << val["hi"] << std::endl;
}
```

### 解析 Json5

```cpp
/***
 * from sample/json5_parse.cpp
***/
#include <iostream>
#include "json5.hpp"

void parsing()
{
    std::string_view content = R"(
// 这是一段json5格式的信息
{
  名字: "MistEO",                  /* key的引号可省略 */
  😊: '😄',                       // emoji为key
  thanks: 'ありがとう',             /* 单引号也可以表示字符串 */
  \u006Bey: ['value',],            // 普通字符和转义可以混用
  inf: +Infinity, nan: NaN,        // 数字可以以"+"开头
  fractional: .3, integer: 42.,    // 小数点作为起始/结尾
  byte_max: 0xff,                  // 十六进制数
  light_speed: +3e8,               // 科学计数法
}
)";
    auto ret = json::parse5(content);
    if (!ret) {
        std::cerr << "Parsing failed" << std::endl;
        return;
    }
    auto& value = ret.value(); // you can use rvalues if needed, like
                               // `auto value = std::move(ret).value();`

    // Output: MistEO
    std::cout << value["名字"] << std::endl;
    // Output: value
    std::string str = (std::string)value["key"][0];
    std::cout << str << std::endl;
    
    // for more json::value usage, please refer to sample.cpp
}
```

### 生成

```cpp
/***
 * from sample/sample.cpp
***/
#include <iostream>
#include "json.hpp"

void serializing()
{
    json::value root;

    root["hello"] = "meojson";
    root["Pi"] = 3.1416;

    root["obj"] = {
        { "obj_key1", "Hi" },
        { "obj_key2", 123 },
        { "obj_key3", true },
    };
    root["obj"].emplace("obj_key4", 789);

    root["obj"].emplace("obj_key5", json::object { { "key4 child", "i am object value" } });
    root["another_obj"]["child"]["grand"] = "i am grand";

    // take union
    root["obj"] |= json::object {
        { "obj_key6", "i am string" },
        { "obj_key7", json::array { "i", "am", "array" } },
    };

    root["arr"] = json::array { 1, 2, 3 };
    root["arr"].emplace(4);
    root["arr"].emplace(5);
    root["arr"] += json::array { 6, 7 };

    std::vector<int> vec = { 1, 2, 3, 4, 5 };
    root["arr from vec"] = json::array(vec);

    std::set<std::string> set = { "a", "bb\n\nb", "cc\t" };
    root["arr from set"] = json::array(set);

    std::map<std::string, int> map {
        { "key1", 1 },
        { "key2", 2 },
    };
    root["obj from map"] = json::object(map);

    std::vector<std::list<std::set<int>>> complex { { { 1, 2, 3 }, { 4, 5 } }, { { 6 }, { 7, 8 } } };
    root["complex"] = json::serialize<false>(complex);

    std::map<std::string, std::map<int, std::vector<double>>> more_complex {
        { "key1", { { 1, { 0.1, 0.2 } }, { 2, { 0.2, 0.3 } } } },
        { "key2", { { 3, { 0.4 } }, { 4, { 0.5, 0.6, 0.7 } } } },
    };
    // the "std::map<int, xxx>" cannot be converted to json because the key is "int",
    // you can set the template parameter "loose" of "serialize" to true, which will make a more relaxed conversion.
    root["more_complex"] = json::serialize<true>(more_complex);

    std::cout << root << std::endl;

    std::ofstream ofs("meo.json");
    ofs << root;
    ofs.close();
}
```
