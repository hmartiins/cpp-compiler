#include "ast.h"

std::string symbolToString(Symbol s) {
    switch(s) {
        case PROGRAM: return "program";
        case FUNC_LIST: return "function_list";
        case FUNC_DECL: return "function_decl";
        case PARAMS: return "params";
        case PARAM_LIST: return "param_list";
        case STATEMENTS: return "statements";
        case STATEMENT: return "statement";
        case EXPR: return "expr";
        case EXPR_P: return "expr'";
        case TERM: return "term";
        case TERM_P: return "term'";
        case FACTOR: return "factor";
        case FACTOR_P: return "factor'";
        case BASE: return "base";
        case ARGS: return "args";
        case ARG_LIST: return "arg_list";
        case T_FUNCTION: return "function";
        case T_RETURN: return "return";
        case T_ID: return "ID";
        case T_NUM: return "NUM";
        case T_PLUS: return "+";
        case T_MINUS: return "-";
        case T_MULT: return "*";
        case T_DIV: return "/";
        case T_POW: return "^";
        case T_ASSIGN: return "=";
        case T_LPAREN: return "(";
        case T_RPAREN: return ")";
        case T_LBRACE: return "{";
        case T_RBRACE: return "}";
        case T_COMMA: return ",";
        case T_SEMICOLON: return ";";
        case T_EOF: return "$";
        case T_EPSILON: return "ε";
        default: return "?";
    }
}

Symbol tokenTypeToSymbol(TokenType type) {
    switch(type) {
        case FUNCTION: return T_FUNCTION;
        case RETURN: return T_RETURN;
        case ID: return T_ID;
        case NUM: return T_NUM;
        case PLUS: return T_PLUS;
        case MINUS: return T_MINUS;
        case MULT: return T_MULT;
        case DIV: return T_DIV;
        case POW: return T_POW;
        case ASSIGN: return T_ASSIGN;
        case LPAREN: return T_LPAREN;
        case RPAREN: return T_RPAREN;
        case LBRACE: return T_LBRACE;
        case RBRACE: return T_RBRACE;
        case COMMA: return T_COMMA;
        case SEMICOLON: return T_SEMICOLON;
        case END_OF_FILE: return T_EOF;
        default: return T_EOF;
    }
}

