

#include "Ethertia.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <ethertia/lang/parser/Parser.h>

class A
{
public:
    int i = 10;
    void sth() {

        Log::info("Sth1");
    }
};

class Mid : public A {

};
class Mid2 : public A {

};

class B : public Mid, public Mid2 {
public:
    void sth() {
        Log::info("Sth2");
    }
};

int main()
{
//    Ethertia().run();

    Lexer lx;
    lx.src = "namespace some::ofns;";
    auto a = Parser::parseCompilationUnit(&lx);



    return 0;
}
