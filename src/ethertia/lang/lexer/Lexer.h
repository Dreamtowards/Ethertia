//
// Created by Dreamtowards on 2022/5/3.
//

#ifndef ETHERTIA_LEXER_H
#define ETHERTIA_LEXER_H

#include <string>

#include "TokenType.h"

class Lexer
{
public:
    int rdi;  // read index.
    std::string src;

    int rdi_begin;  // read index of read-started.

    // result from read.
    TokenType* r_tk;  // Predicted TokenType.
    long r_integer;
    char* r_string;

    /**
     * expected: ptr of TokenType static enums.
     */
    bool read(TokenType* expected)
    {
        // clean previous result.
        r_tk=nullptr; r_integer=0; r_string=nullptr;

        // reach the next valid char/symbol.
        skipBlanksAndComments();
        rdi_begin = rdi;
        
        if (expected)  // Expected TokenType. read or error.
        {
            if (expected->text) {  // is 'constant'
                return startsWith(expected->text);
            } else if (expected == TokenType::L_I32) {
                return readInteger(&r_integer) == 32;
            }
        }
        else  // Predict TokenType.
        {
            for (TokenType* tk : TokenType::ALL) {  // check constant tokens. notice that keywords before identifier.
                if (tk->text && startsWith(tk->text)) {
                    r_tk = tk;
                    return true;
                }
            }

            char ch = src.at(rdi);
            if (isIdentifierChar(ch, true)) {
                // readIdentifier();
            } else if (ch == '\'') {
                // readQuote()
                // s.len==1, r_integer->;
            } else if (ch == '"') {
                // readQuote()
            } else if (isDecimalChar(ch) || (ch=='.' && isDecimalChar(charAt(rdi+1))) ||
                    (ch == '-' && isDecimalChar(charAt(rdi+1))) || (ch == '-' && charAt(rdi+1) == '.' && isDecimalChar(charAt(rdi+2))) ) {
                // readNumber()  // float or integer.
                // int: ("0x"(0-F)+| "0b[01]+" | [1-9][0-9]+) [Ll]?
                // float: [0-9]* '.' [0-9]+ [Dd]?
                // '_' are allowed among digits
            }
        }
    }

    inline bool eof() {
        return rdi < src.length();
    }

    int charAt(int i) const {
        return i < src.length() ? src.at(i) : 0;
    }
    bool startsWith(const char* s) const {
        return src.find(s, rdi) == rdi;
    }
    int encloseOrEnd(const char* search) const {  // findt: find and return the trailing pos. i.e. find(s, search)+strlen(search)
        int i = src.find(search, rdi);
        if (i == std::string::npos)
            return src.length();
        return i + strlen(search);
    }
    
    void skipBlanksAndComments() {
        while (!eof()) {
            char ch = src.at(rdi);
            if (ch <= ' ') {
                rdi++;
            } else if (startsWith("//")) {  // single-line comment.
                rdi = encloseOrEnd("\n");
            } else if (startsWith("/*")) {  // multi-line comment.
                rdi = encloseOrEnd("*/");
            } else {
                break;
            }
        }
    }

    static bool isDecimalChar(int ch) {
        return ch >= '0' && ch <= '9';
    }
    static bool isDigit(int ch, int radix) {
        if (radix == 2) {
            return ch == '0' || ch == '1';
        } else if (radix == 10) {
            return isDecimalChar(ch);
        } else if (radix == 16) {

        }
    }

    static bool isIdentifierChar(int ch, bool first) {
        if (first && isDecimalChar(ch))
            return false;
        return isDecimalChar(ch) || ch=='_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    }

    int readInteger(long* num) {
        int ch1 = charAt(rdi);
        assert(isDecimalChar(ch1));

        const int RDX_DECIMAL = 10, RDX_HEX = 16, RDX_BINARY = 2;
        int rdx = RDX_DECIMAL;  // radix / format

        const int TYP_I32 = 32, TYP_I64 = 64;
        int typ = TYP_I32;

        if (ch1 == '0') {
            int ch2 = charAt(rdi+1);
            if (ch2 == 'x') {
                rdx = RDX_HEX;
            } else if (ch2 == 'b') {
                rdx = RDX_BINARY;
            } else {
                throw "Bad number format. leading 0 but not Hex nor Binary.";
            }
            rdi += 2;
        }

        int numBegin = rdi;
        int numEnd = -1;
        while (!eof()) {
            int ch = charAt(rdi);
            if (ch == '_' || isDigit(ch, rdx)) {
                rdi++;
            } else {
                numEnd = rdi;
                if (ch == 'L') {
                    typ = TYP_I64;
                    rdi++;
                }
                break;
            }
        }
        // if (numBegin == numEnd)  // btw: this is logical impossible.
        //     throw "Bad number format: empty.";
        if (charAt(numBegin) == '_' || charAt(numEnd-1) == '_')
            throw "Bad number format: underscores cannot be on digit boundaries.";

        std::string numStr = src.substr(numBegin, numEnd).replace("_", "");

        if (rdx == RDX_DECIMAL) {

            // *num = ;
        } else if (rdx == RDX_HEX) {

        } else {  // rdx == RDX_BINARY

        }

        return typ;
    }

};

#endif //ETHERTIA_LEXER_H
