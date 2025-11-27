#include "parser_interface.h"
#include "parser.tab.hh"
#include <cstdio>
#include <fstream>
#include <unistd.h>

// Declarações do Flex
extern void reset_lexer();
extern FILE* yyin;
extern int yyparse();

// Variáveis globais
ASTNode* g_ast_root = nullptr;
std::string g_error_msg = "";

ASTNode* parse_source(const std::string& source, std::string& error_msg) {
    // Resetar estado
    g_ast_root = nullptr;
    g_error_msg = "";
    error_msg = "";
    
    // Resetar contadores do lexer
    reset_lexer();
    
    // Criar arquivo temporário para o Flex ler
    char tmp_filename[] = "/tmp/neto_parser_XXXXXX";
    int fd = mkstemp(tmp_filename);
    if (fd == -1) {
        error_msg = "Erro ao criar arquivo temporário";
        return nullptr;
    }
    
    // Escrever código fonte no arquivo temporário
    write(fd, source.c_str(), source.length());
    lseek(fd, 0, SEEK_SET);
    
    // Configurar Flex para ler do arquivo
    FILE* old_yyin = yyin;
    yyin = fdopen(fd, "r");
    
    if (!yyin) {
        close(fd);
        unlink(tmp_filename);
        error_msg = "Erro ao abrir arquivo temporário";
        return nullptr;
    }
    
    int result = yyparse();
    
    // Limpar arquivo temporário
    fclose(yyin);
    unlink(tmp_filename);
    yyin = old_yyin;
    
    // Verificar resultado
    if (result != 0 || !g_ast_root) {
        if (g_error_msg.empty()) {
            error_msg = "Erro de parsing desconhecido";
        } else {
            error_msg = g_error_msg;
        }
        return nullptr;
    }
    
    return g_ast_root;
}

