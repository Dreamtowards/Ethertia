//
// Created by Dreamtowards on 2022/5/4.
//

#include "TokenType.h"

std::vector<TokenType*> TokenType::ALL;


TokenType TokenType::L_IDENTIFIER{};
TokenType TokenType::L_U32{};
TokenType TokenType::L_U64{};
TokenType TokenType::L_I32{};
TokenType TokenType::L_I64{};
TokenType TokenType::L_F32{};
TokenType TokenType::L_F64{};
TokenType TokenType::L_CHAR{};
TokenType TokenType::L_STRING{};

TokenType TokenType::SIZEOF{"sizeof"};
TokenType TokenType::NAMESPACE{"namespace"};
TokenType TokenType::USING{"using"};
TokenType TokenType::NEW{"new"};
TokenType TokenType::IS{"is"};
TokenType TokenType::AS{"as"};
TokenType TokenType::CLASS{"class"};
TokenType TokenType::VAR{"var"};


TokenType TokenType::STATIC{"static"};
TokenType TokenType::CONST{"const"};
TokenType TokenType::NATIVE{"native"};

TokenType TokenType::IF{"if"};
TokenType TokenType::ELSE{"else"};
TokenType TokenType::WHILE{"while"};
TokenType TokenType::CONTINUE{"continue"};
TokenType TokenType::BREAK{"break"};
TokenType TokenType::RETURN{"return"};
TokenType TokenType::FOR{"for"};
TokenType TokenType::GOTO{"goto"};

TokenType TokenType::TRUE{"true"};
TokenType TokenType::FALSE{"false"};
TokenType TokenType::NULLPTR{"nullptr"};
//TokenType TokenType::THIS{"this"};


TokenType TokenType::AMPAMP{"&&"};
TokenType TokenType::AMP{"&"};
TokenType TokenType::BARBAR{"||"};
TokenType TokenType::BAR{"|"};
TokenType TokenType::COLCOL{"::"};
TokenType TokenType::COL{":"};
TokenType TokenType::DOTDOTDOT{"..."};
TokenType TokenType::DOT{"."};
TokenType TokenType::PLUSEQ{"+="};
TokenType TokenType::PLUSPLUS{"++"};
TokenType TokenType::PLUS{"+"};
TokenType TokenType::ARROW{"->"};
TokenType TokenType::SUBEQ{"-="};
TokenType TokenType::SUBSUB{"--"};
TokenType TokenType::SUB{"-"};

TokenType TokenType::EQEQ{"=="};
TokenType TokenType::EQ{"="};
TokenType TokenType::BANGEQ{"!="};
TokenType TokenType::BANG{"!"};
TokenType TokenType::GTEQ{">="};
TokenType TokenType::GTGT{">>"};
TokenType TokenType::GT{">"};
TokenType TokenType::LTEQ{"<="};
TokenType TokenType::LTLT{"<<"};
TokenType TokenType::LT{"<"};


TokenType TokenType::QUES{"?"};
TokenType TokenType::CARET{"^"};
TokenType TokenType::TILDE{"~"};
TokenType TokenType::AT{"@"};
TokenType TokenType::PERCENT{"%"};
TokenType TokenType::COMMA{","};
TokenType TokenType::STAREQ{"*="};
TokenType TokenType::STAR{"*"};
TokenType TokenType::SLASHEQ{"/="};
TokenType TokenType::SLASH{"/"};
TokenType TokenType::SEMI{";"};

TokenType TokenType::LPAREN{"("};
TokenType TokenType::RPAREN{")"};
TokenType TokenType::LBRACE{"{"};
TokenType TokenType::RBRACE{"}"};
TokenType TokenType::LBRACKET{"["};
TokenType TokenType::RBRACKET{"]"};

