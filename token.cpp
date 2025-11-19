#include "token.h"

std::string tokenTypeToString(TokenType type) {
    switch(type) {
        case FUNCTION: return "FUNCTION";
        case RETURN: return "RETURN";
        case ID: return "ID";
        case NUM: return "NUM";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MULT: return "MULT";
        case DIV: return "DIV";
        case POW: return "POW";
        case ASSIGN: return "ASSIGN";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case COMMA: return "COMMA";
        case SEMICOLON: return "SEMICOLON";
        case END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}

