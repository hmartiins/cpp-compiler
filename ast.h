#ifndef AST_H
#define AST_H

#include "token.h"
#include <string>
#include <vector>

// Símbolos da gramática
enum Symbol {
    // Não-terminais
    PROGRAM, FUNC_LIST, FUNC_DECL, PARAMS, PARAM_LIST, 
    STATEMENTS, STATEMENT, EXPR, EXPR_P, TERM, TERM_P, 
    FACTOR, FACTOR_P, BASE, ARGS, ARG_LIST,
    // Terminais (correspondem aos tokens)
    T_FUNCTION, T_RETURN, T_ID, T_NUM, T_PLUS, T_MINUS, 
    T_MULT, T_DIV, T_POW, T_ASSIGN, T_LPAREN, T_RPAREN, 
    T_LBRACE, T_RBRACE, T_COMMA, T_SEMICOLON, T_EOF, T_EPSILON
};

std::string symbolToString(Symbol s);
Symbol tokenTypeToSymbol(TokenType type);

// Árvore sintática abstrata (AST)
struct ASTNode {
    Symbol symbol;
    std::string value;
    std::vector<ASTNode*> children;
    
    ASTNode(Symbol s, std::string v = "") : symbol(s), value(v) {}
    
    ~ASTNode() {
        for (auto child : children) {
            delete child;
        }
    }
    
    void addChild(ASTNode* child) {
        children.push_back(child);
    }
};

#endif // AST_H

