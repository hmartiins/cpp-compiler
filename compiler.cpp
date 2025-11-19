#include "compiler.h"
#include "lexer.h"
#include "parser.h"
#include "semantic.h"
#include "token.h"
#include <iostream>
#include <fstream>
#include <sstream>

Compiler::Compiler(bool v) : verbose(v) {}

bool Compiler::compile(const std::string& source) {
    sourceCode = source;
    
    // Fase 1: Análise Léxica
    if (verbose) std::cout << "=== ANÁLISE LÉXICA ===" << std::endl;
    Lexer lexer(sourceCode);
    std::vector<Token> tokens = lexer.tokenize();
    
    // Verificar erros léxicos
    for (const auto& tok : tokens) {
        if (tok.type == UNKNOWN) {
            std::cout << "REJEITADO" << std::endl;
            std::cout << "Erro léxico na linha " << tok.line << ", coluna " << tok.column 
                     << ": caractere inválido '" << tok.lexeme << "'" << std::endl;
            return false;
        }
    }
    
    if (verbose) {
        for (const auto& tok : tokens) {
            std::cout << tokenTypeToString(tok.type) << " ";
            if (tok.type == ID || tok.type == NUM) {
                std::cout << "(" << tok.lexeme << ")";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    
    // Fase 2: Análise Sintática
    if (verbose) std::cout << "=== ANÁLISE SINTÁTICA ===" << std::endl;
    Parser parser(tokens);
    ASTNode* ast = parser.parse();
    
    if (!ast) {
        std::cout << "REJEITADO" << std::endl;
        std::cout << parser.getError() << std::endl;
        return false;
    }
    
    if (verbose) std::cout << "Árvore sintática construída com sucesso." << std::endl << std::endl;
    
    // Fase 3: Análise Semântica
    if (verbose) std::cout << "=== ANÁLISE SEMÂNTICA ===" << std::endl;
    SemanticAnalyzer semantic;
    bool semanticOk = semantic.analyze(ast);
    
    if (!semanticOk) {
        std::cout << "REJEITADO" << std::endl;
        std::cout << semantic.getError() << std::endl;
        delete ast;
        return false;
    }
    
    if (verbose) std::cout << "Análise semântica concluída com sucesso." << std::endl << std::endl;
    
    // Sucesso!
    std::cout << "ACEITO" << std::endl;
    
    delete ast;
    return true;
}

bool Compiler::compileFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Erro: não foi possível abrir o arquivo '" << filename << "'" << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    sourceCode = buffer.str();
    file.close();
    
    return compile(sourceCode);
}

