//
// Created by Dreamtowards on 2022/5/3.
//

#ifndef ETHERTIA_LEXER_H
#define ETHERTIA_LEXER_H

#include <string>
#include <sstream>
#include <ethertia/util/Strings.h>

#include <ethertia/lang/lexer/TokenType.h>

class Lexer
{
public:
    int rdi = 0;  // read index.
    std::string src;
    std::string src_name;  // just memo desc.

    int rdi_begin;  // read index of read-started.

    // result from read.
    TokenType* r_tk;    // Predicted TokenType.
    long r_integer;     // u8, u16, u32, u64, i8, i16, i32, i64, charliteral.
    double r_fp;        // f32, f64.
    std::string r_str;  // stringliteral, identifier.

    /**
     * expected: ptr of TokenType static enums.
     */
    bool read(TokenType* expected)
    {
        // clean previous result.
        r_tk=nullptr; r_integer=0; r_fp=0; r_str.clear();

        if (eof()) return false;

        // reach the next valid char/symbol.
        skipBlanksAndComments();
        rdi_begin = rdi;
        
        if (expected)  // Expected TokenType. read or error.
        {
            if (expected->text) {  // is 'constant token'.
                return startsWith_Jmp(expected->text);
            } else if (expected == &TokenType::L_IDENTIFIER) {
                r_str = readIdentifier();
                return true;
            } else if (expected == &TokenType::L_CHAR) {
                r_integer = readChar();
                return true;
            } else if (expected == &TokenType::L_STRING) {
                r_str = readQuote('"');
                return true;
            } else if (TokenType::isNumber(expected)) {
                return readNumber(&r_integer, &r_fp) == expected;
            } else {
                throw "Unknown TokenType.";
            }
        }
        else  // Predict TokenType.
        {
            for (TokenType* tk : TokenType::ALL) {  // check constant tokens. notice that keywords before identifier.
                if (tk->text && startsWith(tk->text)) {
                    r_tk = tk;
                    rdi += strlen(tk->text);
                    return true;
                }
            }

            int ch = charAt(rdi);
            if (isIdentifierChar(ch, true)) {
                r_tk = &TokenType::L_IDENTIFIER;
                r_str = readIdentifier();
            } else if (ch == '\'') {
                r_tk = &TokenType::L_CHAR;
                r_integer = readChar();
            } else if (ch == '"') {
                r_tk = &TokenType::L_STRING;
                r_str = readQuote('"');
            } else if (_briefStartsWithNumber()) {
                r_tk = readNumber(&r_integer, &r_fp);
            } else {
                throw "Unrecognizable input or character.";
            }
            return true;
        }
    }

    // match and move
    void next(TokenType* tk) {
        if (!read(tk))  // might error.
            throw "Unexpected token.";
    }
    // move
    void next() {
        read(nullptr);
    }
    // move if match. trynext()
    bool nexting(TokenType* tk) {
        int mark = rdi;
        if (read(tk)) {
            return true;
        } else {
            rdi = mark;
            return false;
        }
    }

    TokenType* trynext(const std::vector<TokenType*>& ls) {
        int mark = rdi;
        next();
        for (TokenType* tk : ls) {
            if (tk == r_tk)
                return tk;
        }
        rdi = mark;
        return nullptr;
    }
    TokenType* trynext(TokenType* tk) {
        int mark = rdi;
        next();
        if (tk == r_tk)
            return r_tk;
        rdi = mark;
        return nullptr;
    }

    // test match, not move
    bool peeking(TokenType* tk) {
        int mark = rdi;
        bool r = read(tk);
        rdi = mark;
        return r;
    }
    // get next, not move
    TokenType* peek() {
        int mark = rdi;
        read(nullptr);
        rdi = mark;
        return r_tk;
    }



    bool _briefStartsWithNumber() {
        int ch = charAt(rdi);
        int ch1 = charAt(rdi+1);
        return isDecimalChar(ch) || (ch=='.' && isDecimalChar(ch1)) ||
                (ch == '-' && isDecimalChar(ch1)) || (ch == '-' && ch1 == '.' && isDecimalChar(charAt(rdi+2)));
    }

    inline bool eof() const {
        return rdi >= src.length();
    }

    int charAt(int i) const {
        return i < src.length() ? src.at(i) : 0;
    }
    bool startsWith(const char* s) const {
        return src.find(s, rdi) == rdi;
    }
    bool startsWith_Jmp(const char* s) {
        if (startsWith(s)) {
            rdi += strlen(s);
            return true;
        } else {
            return false;
        }
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
            return (ch >= '0' && ch <= '9') || (ch >= 'A' && ch <= 'F');
        } else {
            throw "Unsupported radix.";
        }
    }

    static bool isIdentifierChar(int ch, bool first) {
        if (first && isDecimalChar(ch))
            return false;
        return isDecimalChar(ch) || ch=='_' || (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
    }

    TokenType* readNumber(long* numI, double* numFP) {
        bool neg = false;
        if (charAt(rdi) == '-') {
            rdi++;
            neg = true;
        }
        int ch1 = charAt(rdi);
        if (!isDecimalChar(ch1) && ch1 != '.')
            throw "Bad number format: not a number.";

        TokenType* typ = TokenType::DEF_I;

        const int FMT_DECIMAL = 10, FMT_HEX = 16, FMT_BINARY = 2;  // hex/bin radix/format is for integers. float point number can only use Decimal format.
        int fmt = FMT_DECIMAL;

        if (ch1 == '0') {  // format read for integers.
            int ch2 = charAt(rdi+1);
            if (ch2 == 'x') {
                fmt = FMT_HEX;
                rdi += 2;
            } else if (ch2 == 'b') {
                fmt = FMT_BINARY;
                rdi += 2;
            } else if (isDecimalChar(ch2)) {
                throw "Bad number format: leading 0 decimal.";
            }
        }

        int begin = rdi;
        int end = -1;
        bool dot = false;
        while (!eof()) {
            int ch = charAt(rdi);
            if (ch == '.' && isDecimalChar(charAt(rdi+1)) && !dot) {
                dot = true;
                typ = TokenType::DEF_F;
                rdi++;
            } else if (isDigit(ch, fmt) || ch == '_') {
                rdi++;
            } else {
                end = rdi;

                if (ch == 'E') {  // FP exponent.
                    typ = TokenType::DEF_F;
                    rdi++;
                    if (charAt(rdi) == '-') rdi++;
                    if (!isDecimalChar(charAt(rdi)))
                        throw "Bad number format: bad fp exponent, not a decimal number.";

                    while (!eof() && isDecimalChar(charAt(rdi))) {
                        rdi++;
                    }
                }

                // u8, u16, u32, u64; i8, i16, i32, i64; f32, f64
                TokenType* suftyp = typ;
                     if (startsWith_Jmp("u32")) suftyp = &TokenType::L_U32;
                else if (startsWith_Jmp("u64")) suftyp = &TokenType::L_U64;
                else if (startsWith_Jmp("i32")) suftyp = &TokenType::L_I32;
                else if (startsWith_Jmp("i64")) suftyp = &TokenType::L_I64;
                else if (startsWith_Jmp("f32")) suftyp = &TokenType::L_F32;
                else if (startsWith_Jmp("f64")) suftyp = &TokenType::L_F64;

                if (TokenType::isFp(typ) && !TokenType::isFp(suftyp))
                    throw "Bad number format: illegal suffix, FP cannot as integer.";
                typ = suftyp;

                break;
            }
        }
        if (TokenType::isFp(typ) && fmt != FMT_DECIMAL)
            throw "Bad number format: float point number can only use decimal 10 radix format.";
        if (charAt(begin) == '_' || charAt(end-1) == '_')  // need this limitation?
            throw "Bad number format: underscores cannot be on digit boundaries.";

        std::string nstr = src.substr(begin, end-begin);
        Strings::erase(nstr, '_');

        if (nstr.length() == 0)
            throw "Bad number format: no digit.";

        if (TokenType::isFp(typ)) {
            *numFP = std::stod(nstr);  // 10.2e-2
        } else {
            *numI = std::stol(nstr, 0, fmt);
        }

        return typ;
    }

    std::string readIdentifier() {
        int begin = rdi;
        if (isIdentifierChar(charAt(begin), true)) {
            rdi++;
        } else {
            throw "Bad identifier: not an identifier.";
        }
        while (!eof()) {
            int ch = charAt(rdi);
            if (isIdentifierChar(ch, false)) {
                rdi++;
            } else {
                break;
            }
        }
        return src.substr(begin, rdi-begin);
    }


    // linebreak char is allowed.
    std::string readQuote(int quote) {
        if (charAt(rdi) != quote)
            throw "Bad quote: wrong begin.";  // ...
        rdi++;

        std::stringstream buf;

        while (!eof()) {
            int ch = charAt(rdi);
            if (ch == quote) {  // enclose.
                rdi++;
                break;
            } else if (ch == '\\') {  // escape.
                int ch1 = charAt(rdi+1);
                rdi += 2;

                     if (ch1 == '\\') buf << '\\';  // backslash
                else if (ch1 == 'n')  buf << '\n';  // linebreak
                else if (ch1 == 't')  buf << '\t';  // tab
                else if (ch1 == '\'') buf << '\'';  // single quote
                else if (ch1 == '"')  buf << '"';   // double quote
                else if (ch1 == '0')  buf << '\0';  // end of str.
                else if (ch1 == '\n') {}            // nop. ignore linebreak.
                else if (ch1 == 'u') {  // character unicode. 4 hex representation.
                    throw "character codepoint is unsupported.";
                } else {
                    throw "Illegal escape.";
                }
            } else {
                buf << (char)ch;
                rdi++;
            }
        }

        return buf.str();
    }

    int readChar() {
        std::string s = readQuote('\'');
        if (s.length() != 1)
            throw "Bad character literal: should be one char.";
        return s.at(0);
    }







    class SourceSegment {
    public:
        Lexer* lex;
        int begin;
        int end;
    };

    int rdi_clean() {
        skipBlanksAndComments();
        return rdi;
    }

    bool clean_eof() {
        rdi_clean();
        return eof();
    }

    std::stack<int> rdi_stack;

    int push_rdic() {
        int i = rdi_clean();
        rdi_stack.push(i);
        return i;
    }
    SourceSegment pop_rdic() {
        int beg = rdi_stack.top();
        rdi_stack.pop();
        return {this, beg, rdi};
    }

};

#endif //ETHERTIA_LEXER_H
