#ifndef PARSER_INTERFACE_H
#define PARSER_INTERFACE_H

#include "ast.h"
#include <string>

// Interface simples para o parser Bison/Flex
// Funções implementadas em parser.y e lexer.l

// Função principal de parsing (implementada em parser.tab.cc pelo Bison)
extern int yyparse();

// Funções do Flex (implementadas em lex.yy.cc)
extern void reset_lexer();
extern FILE* yyin;

// Variáveis globais do parser (definidas em parser_interface.cpp)
extern ASTNode* g_ast_root;
extern std::string g_error_msg;

// Função wrapper para parsing
ASTNode* parse_source(const std::string& source, std::string& error_msg);

#endif // PARSER_INTERFACE_H

