

#include "Ethertia.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <ethertia/lang/lexer/Lexer.h>
#include <ethertia/lang/lexer/TokenType.h>
#include <ethertia/util/Strings.h>

int main()
{
//    Ethertia().run();


//    TokenType* a = &TokenType::SIZEOF;
//    Log::info("ADDR: {}, {} {} {}", &TokenType::EoF, &TokenType::L_STRING, &TokenType::L_CHAR, &TokenType::L_I32);
//    for (int i = 0; i < 4; ++i) {
//        Log::info("Con: ", a[i].text);
//    }


    Lexer lx;
    lx.src = "_SomeId of src 100.0 + \"STR content\\\nbr\"5 'a'";

    while (lx.read(nullptr)) {
        auto* tk = lx.r_tk;

        std::string rd = lx.src.substr(lx.rdi_begin, lx.rdi-lx.rdi_begin);
        Log::info("S: ", rd);
        if (tk->text) {
            Log::info("Read: ", lx.r_tk->text);
        } else {
            Log::info("Read i: {}, f: {}, s: {}", lx.r_integer, lx.r_fp, lx.r_str);
        }
        Log::info("");
    }

    return 0;
}
