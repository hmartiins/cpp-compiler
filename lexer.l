%{
#include "token.h"
#include "ast.h"
#include "parser.tab.hh"
#include <cstdlib>
#include <string>
#include <sstream>

extern int yylineno;
extern int yycolumn;
extern char* yytext;

// Função auxiliar para converter token do Bison para TokenType
TokenType bison_token_to_type(int bison_token) {
    switch(bison_token) {
        case FUNCTION_TOKEN: return FUNCTION;
        case RETURN_TOKEN: return RETURN;
        case ID_TOKEN: return ID;
        case NUM_TOKEN: return NUM;
        case PLUS_TOKEN: return PLUS;
        case MINUS_TOKEN: return MINUS;
        case MULT_TOKEN: return MULT;
        case DIV_TOKEN: return DIV;
        case POW_TOKEN: return POW;
        case ASSIGN_TOKEN: return ASSIGN;
        case LPAREN_TOKEN: return LPAREN;
        case RPAREN_TOKEN: return RPAREN;
        case LBRACE_TOKEN: return LBRACE;
        case RBRACE_TOKEN: return RBRACE;
        case COMMA_TOKEN: return COMMA;
        case SEMICOLON_TOKEN: return SEMICOLON;
        case UNKNOWN_TOKEN: return UNKNOWN;
        default: return UNKNOWN;
    }
}

// Função auxiliar para criar token
#define TOKEN(t) do { \
    yylval.token_val = new Token(bison_token_to_type(t), std::string(yytext), yylineno, yycolumn); \
    yycolumn += yyleng; \
    return t; \
} while(0)

%}

%option noyywrap
%option yylineno

%{
// Variável global para rastrear coluna
int yycolumn = 1;
%}

%%

[ \t]+          { yycolumn += yyleng; }
\n              { yylineno++; yycolumn = 1; }
"//".*          { /* comentário de linha */ }

"func"          { TOKEN(FUNCTION_TOKEN); }
"return"        { TOKEN(RETURN_TOKEN); }

[0-9]+          { TOKEN(NUM_TOKEN); }
[a-zA-Z_][a-zA-Z0-9_]* { TOKEN(ID_TOKEN); }

"+"             { TOKEN(PLUS_TOKEN); }
"-"             { TOKEN(MINUS_TOKEN); }
"*"             { TOKEN(MULT_TOKEN); }
"/"             { TOKEN(DIV_TOKEN); }
"^"             { TOKEN(POW_TOKEN); }
"="             { TOKEN(ASSIGN_TOKEN); }

"("             { TOKEN(LPAREN_TOKEN); }
")"             { TOKEN(RPAREN_TOKEN); }
"{"             { TOKEN(LBRACE_TOKEN); }
"}"             { TOKEN(RBRACE_TOKEN); }
","             { TOKEN(COMMA_TOKEN); }
";"             { TOKEN(SEMICOLON_TOKEN); }

.               { TOKEN(UNKNOWN_TOKEN); }

<<EOF>>         { return 0; }

%%

// Função para resetar o lexer
void reset_lexer() {
    yycolumn = 1;
    yylineno = 1;
}
