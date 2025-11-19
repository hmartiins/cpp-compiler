#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// ==================== ANALISADOR LÉXICO ====================

enum TokenType {
    // Palavras-chave
    FUNCTION, RETURN,
    // Identificadores e literais
    ID, NUM,
    // Operadores
    PLUS, MINUS, MULT, DIV, POW, ASSIGN,
    // Delimitadores
    LPAREN, RPAREN, LBRACE, RBRACE, COMMA, SEMICOLON,
    // Especiais
    END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    std::string lexeme;
    int line;
    int column;
    
    Token(TokenType t = UNKNOWN, std::string lex = "", int l = 1, int c = 1) 
        : type(t), lexeme(lex), line(l), column(c) {}
};

std::string tokenTypeToString(TokenType type);

#endif // TOKEN_H

