#include "lexer.h"
#include <cctype>

char Lexer::peek(int offset) {
    if (pos + offset >= input.length()) return '\0';
    return input[pos + offset];
}

char Lexer::advance() {
    if (pos >= input.length()) return '\0';
    char ch = input[pos++];
    if (ch == '\n') {
        line++;
        column = 1;
    } else {
        column++;
    }
    return ch;
}

void Lexer::skipWhitespace() {
    while (isspace(peek())) {
        advance();
    }
}

void Lexer::skipComment() {
    if (peek() == '/' && peek(1) == '/') {
        // Comentário de linha
        while (peek() != '\n' && peek() != '\0') {
            advance();
        }
    } else if (peek() == '/' && peek(1) == '*') {
        // Comentário de bloco
        advance(); // /
        advance(); // *
        while (true) {
            if (peek() == '\0') break;
            if (peek() == '*' && peek(1) == '/') {
                advance(); // *
                advance(); // /
                break;
            }
            advance();
        }
    }
}

Token Lexer::readNumber() {
    int startCol = column;
    std::string num;
    bool hasDecimal = false;
    
    while (isdigit(peek()) || peek() == '.') {
        if (peek() == '.') {
            if (hasDecimal) break; // Segundo ponto, para
            hasDecimal = true;
        }
        num += advance();
    }
    
    return Token(NUM, num, line, startCol);
}

Token Lexer::readIdentifier() {
    int startCol = column;
    std::string id;
    
    while (isalnum(peek()) || peek() == '_') {
        id += advance();
    }
    
    // Verificar se é palavra-chave
    if (keywords.find(id) != keywords.end()) {
        return Token(keywords[id], id, line, startCol);
    }
    
    return Token(ID, id, line, startCol);
}

Lexer::Lexer(const std::string& source) : input(source), pos(0), line(1), column(1) {
    // Inicializar palavras-chave
    keywords["func"] = FUNCTION;
    keywords["return"] = RETURN;
}

Token Lexer::nextToken() {
    skipWhitespace();
    
    // Pular comentários
    while ((peek() == '/' && peek(1) == '/') || (peek() == '/' && peek(1) == '*')) {
        skipComment();
        skipWhitespace();
    }
    
    if (peek() == '\0') {
        return Token(END_OF_FILE, "", line, column);
    }
    
    int startCol = column;
    char ch = peek();
    
    // Números
    if (isdigit(ch)) {
        return readNumber();
    }
    
    // Identificadores e palavras-chave
    if (isalpha(ch) || ch == '_') {
        return readIdentifier();
    }
    
    // Operadores e delimitadores
    advance();
    switch (ch) {
        case '+': return Token(PLUS, "+", line, startCol);
        case '-': return Token(MINUS, "-", line, startCol);
        case '*': return Token(MULT, "*", line, startCol);
        case '/': return Token(DIV, "/", line, startCol);
        case '^': return Token(POW, "^", line, startCol);
        case '=': return Token(ASSIGN, "=", line, startCol);
        case '(': return Token(LPAREN, "(", line, startCol);
        case ')': return Token(RPAREN, ")", line, startCol);
        case '{': return Token(LBRACE, "{", line, startCol);
        case '}': return Token(RBRACE, "}", line, startCol);
        case ',': return Token(COMMA, ",", line, startCol);
        case ';': return Token(SEMICOLON, ";", line, startCol);
        default:
            return Token(UNKNOWN, std::string(1, ch), line, startCol);
    }
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    Token tok;
    do {
        tok = nextToken();
        tokens.push_back(tok);
    } while (tok.type != END_OF_FILE);
    return tokens;
}

