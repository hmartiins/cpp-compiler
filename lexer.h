#ifndef LEXER_H
#define LEXER_H

#include "token.h"
#include <string>
#include <vector>
#include <map>

class Lexer {
private:
    std::string input;
    size_t pos;
    int line;
    int column;
    std::map<std::string, TokenType> keywords;
    
    char peek(int offset = 0);
    char advance();
    void skipWhitespace();
    void skipComment();
    Token readNumber();
    Token readIdentifier();
    
public:
    Lexer(const std::string& source);
    Token nextToken();
    std::vector<Token> tokenize();
};

#endif // LEXER_H

