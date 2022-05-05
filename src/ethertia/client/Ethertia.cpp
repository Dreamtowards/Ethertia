

#include "Ethertia.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <ethertia/lang/parser/Parser.h>

int main()
{
//    Ethertia().run();

    Lexer lx;
    lx.src = "_SomeId of src 100.0 + \"STR content\\\nbr\"5 'a'";

    auto a = Parser::parseCompilationUnit(&lx);



    return 0;
}
