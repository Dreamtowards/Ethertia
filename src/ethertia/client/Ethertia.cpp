

#include "Ethertia.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <ethertia/lang/parser/Parser.h>

int main()
{
    Ethertia().run();

//    func<int, int, float>();

//    Lexer lx;
//    lx.src = "namespace some::ofns;";
//    auto a = Parser::parseCompilationUnit(&lx);



    return 0;
}
