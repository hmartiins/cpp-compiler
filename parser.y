%{
#include "ast.h"
#include "token.h"
#include "utils.h"
#include <cstdio>
#include <cstdlib>
#include <string>

extern int yylex();
extern int yylineno;
extern int yycolumn;
extern char* yytext;
extern FILE* yyin;

// Variáveis globais para comunicação (definidas em parser_interface.cpp)
extern ASTNode* g_ast_root;
extern std::string g_error_msg;

// Função de erro do Bison
void yyerror(const char* s) {
    g_error_msg = logError("[ERROR] " + std::string(s) + " na linha " + std::to_string(yylineno) + 
                          ", coluna " + std::to_string(yycolumn));
}

%}

%union {
    ASTNode* node_val;
    Token* token_val;
    std::string* string_val;
    int int_val;
}

%token <token_val> FUNCTION_TOKEN RETURN_TOKEN ID_TOKEN NUM_TOKEN
%token <token_val> PLUS_TOKEN MINUS_TOKEN MULT_TOKEN DIV_TOKEN POW_TOKEN ASSIGN_TOKEN
%token <token_val> LPAREN_TOKEN RPAREN_TOKEN LBRACE_TOKEN RBRACE_TOKEN COMMA_TOKEN SEMICOLON_TOKEN
%token <token_val> UNKNOWN_TOKEN

%type <node_val> program function_list function_decl params param_list
%type <node_val> statements statement expr expr_p term term_p factor factor_p base args arg_list

%left PLUS_TOKEN MINUS_TOKEN
%left MULT_TOKEN DIV_TOKEN
%right POW_TOKEN

%start program

%%

program:
    function_list {
        $$ = new ASTNode(PROGRAM);
        $$->addChild($1);
        g_ast_root = $$;
    }
    ;

function_list:
    function_decl function_list {
        $$ = new ASTNode(FUNC_LIST);
        $$->addChild($1);
        $$->addChild($2);
    }
    | /* epsilon */ {
        $$ = new ASTNode(FUNC_LIST);
    }
    ;

function_decl:
    FUNCTION_TOKEN ID_TOKEN LPAREN_TOKEN params RPAREN_TOKEN LBRACE_TOKEN statements RBRACE_TOKEN {
        $$ = new ASTNode(FUNC_DECL);
        $$->addChild(new ASTNode(T_ID, $2->lexeme));
        $$->addChild($4);
        $$->addChild($7);
        delete $2;
    }
    ;

params:
    param_list {
        $$ = new ASTNode(PARAMS);
        $$->addChild($1);
    }
    | /* epsilon */ {
        $$ = new ASTNode(PARAMS);
    }
    ;

param_list:
    ID_TOKEN {
        ASTNode* node = new ASTNode(PARAM_LIST);
        node->addChild(new ASTNode(T_ID, $1->lexeme));
        $$ = node;
        delete $1;
    }
    | ID_TOKEN COMMA_TOKEN param_list {
        ASTNode* node = new ASTNode(PARAM_LIST);
        node->addChild(new ASTNode(T_ID, $1->lexeme));
        node->addChild($3);
        $$ = node;
        delete $1;
    }
    ;

statements:
    statement statements {
        $$ = new ASTNode(STATEMENTS);
        $$->addChild($1);
        $$->addChild($2);
    }
    | /* epsilon */ {
        $$ = new ASTNode(STATEMENTS);
    }
    ;

statement:
    RETURN_TOKEN expr SEMICOLON_TOKEN {
        $$ = new ASTNode(STATEMENT);
        $$->addChild(new ASTNode(T_RETURN, "return"));
        $$->addChild($2);
    }
    | ID_TOKEN ASSIGN_TOKEN expr SEMICOLON_TOKEN {
        $$ = new ASTNode(STATEMENT);
        $$->addChild(new ASTNode(T_ID, $1->lexeme));
        $$->addChild($3);
        delete $1;
    }
    ;

expr:
    term expr_p {
        $$ = new ASTNode(EXPR);
        $$->addChild($1);
        $$->addChild($2);
    }
    ;

expr_p:
    PLUS_TOKEN term expr_p {
        $$ = new ASTNode(EXPR_P);
        $$->addChild(new ASTNode(T_PLUS, "+"));
        $$->addChild($2);
        $$->addChild($3);
    }
    | MINUS_TOKEN term expr_p {
        $$ = new ASTNode(EXPR_P);
        $$->addChild(new ASTNode(T_MINUS, "-"));
        $$->addChild($2);
        $$->addChild($3);
    }
    | /* epsilon */ {
        $$ = new ASTNode(EXPR_P);
    }
    ;

term:
    factor term_p {
        $$ = new ASTNode(TERM);
        $$->addChild($1);
        $$->addChild($2);
    }
    ;

term_p:
    MULT_TOKEN factor term_p {
        $$ = new ASTNode(TERM_P);
        $$->addChild(new ASTNode(T_MULT, "*"));
        $$->addChild($2);
        $$->addChild($3);
    }
    | DIV_TOKEN factor term_p {
        $$ = new ASTNode(TERM_P);
        $$->addChild(new ASTNode(T_DIV, "/"));
        $$->addChild($2);
        $$->addChild($3);
    }
    | /* epsilon */ {
        $$ = new ASTNode(TERM_P);
    }
    ;

factor:
    base factor_p {
        $$ = new ASTNode(FACTOR);
        $$->addChild($1);
        $$->addChild($2);
    }
    ;

factor_p:
    POW_TOKEN base factor_p {
        $$ = new ASTNode(FACTOR_P);
        $$->addChild(new ASTNode(T_POW, "^"));
        $$->addChild($2);
        $$->addChild($3);
    }
    | /* epsilon */ {
        $$ = new ASTNode(FACTOR_P);
    }
    ;

base:
    NUM_TOKEN {
        $$ = new ASTNode(BASE);
        $$->addChild(new ASTNode(T_NUM, $1->lexeme));
        delete $1;
    }
    | ID_TOKEN {
        $$ = new ASTNode(BASE);
        $$->addChild(new ASTNode(T_ID, $1->lexeme));
        delete $1;
    }
    | ID_TOKEN LPAREN_TOKEN args RPAREN_TOKEN {
        $$ = new ASTNode(BASE);
        $$->addChild(new ASTNode(T_ID, $1->lexeme));
        $$->addChild($3);
        delete $1;
    }
    | LPAREN_TOKEN expr RPAREN_TOKEN {
        $$ = new ASTNode(BASE);
        $$->addChild($2);
    }
    ;

args:
    arg_list {
        $$ = new ASTNode(ARGS);
        $$->addChild($1);
    }
    | /* epsilon */ {
        $$ = new ASTNode(ARGS);
    }
    ;

arg_list:
    expr {
        $$ = new ASTNode(ARG_LIST);
        $$->addChild($1);
    }
    | expr COMMA_TOKEN arg_list {
        $$ = new ASTNode(ARG_LIST);
        $$->addChild($1);
        $$->addChild($3);
    }
    ;

%%

