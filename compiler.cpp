#include "compiler.h"
#include "parser_interface.h"
#include "semantic.h"
#include "codegen.h"
#include "utils.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

Compiler::Compiler(bool v) : verbose(v) {}

bool Compiler::compile(const std::string& source) {
    sourceCode = source;
    
    // Fase 1 e 2: Análise Léxica e Sintática (usando Bison/Flex)
    // O Bison/Flex fazem ambas as análises em conjunto
    if (verbose) std::cout << "=== ANÁLISE LÉXICA E SINTÁTICA (LR com Bison) ===" << std::endl;
    
    std::string parserError;
    ASTNode* ast = parse_source(source, parserError);
    
    if (!ast || !parserError.empty()) {
        std::cout << logError("REJEITADO") << std::endl;
        if (!parserError.empty()) {
            std::cout << parserError << std::endl;
        } else {
            std::cout << logError("[ERROR] Erro na análise sintática") << std::endl;
        }
        return false;
    }
    
    if (verbose) std::cout << logSuccess("[SUCCESS] Árvore sintática construída com sucesso usando parser LR (Bison).") << std::endl << std::endl;
    
    // Fase 3: Análise Semântica
    if (verbose) std::cout << "=== ANÁLISE SEMÂNTICA ===" << std::endl;
    SemanticAnalyzer semantic;
    bool semanticOk = semantic.analyze(ast);
    
    if (!semanticOk) {
        std::cout << logError("REJEITADO") << std::endl;
        std::cout << semantic.getError() << std::endl;
        delete ast;
        return false;
    }
    
    if (verbose) std::cout << logSuccess("[SUCCESS] Análise semântica concluída com sucesso.") << std::endl << std::endl;
    
    // Fase 4: Geração de Código Intermediário
    if (verbose) std::cout << "=== GERAÇÃO DE CÓDIGO INTERMEDIÁRIO ===" << std::endl;
    CodeGenerator codegen;
    std::vector<ThreeAddressCode> intermediateCode = codegen.generate(ast);
    
    if (verbose) {
        std::cout << logSuccess("[SUCCESS] Código intermediário gerado com sucesso.") << std::endl;
        std::cout << "\n--- Código de Três Endereços ---\n";
        std::cout << codegen.toString();
        std::cout << "--- Fim do Código Intermediário ---\n\n";
    }
    
    std::cout << logSuccess("[SUCCESS] Compilação concluída com sucesso.") << std::endl;
    
    delete ast;
    return true;
}

bool Compiler::compileFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << logError("[ERROR] Não foi possível abrir o arquivo '" + filename + "'") << std::endl;
        return false;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    sourceCode = buffer.str();
    file.close();
    
    bool result = compile(sourceCode);
    
    // Se compilação foi bem-sucedida, salvar código intermediário em arquivo
    if (result) {
        // Gerar código intermediário novamente para salvar
        std::string parserError;
        ASTNode* ast = parse_source(sourceCode, parserError);
        if (ast) {
            CodeGenerator codegen;
            codegen.generate(ast);
            
            // Criar nome do arquivo de saída: nome.ir
            std::string outputFile = filename;
            size_t pos = outputFile.find_last_of('.');
            if (pos != std::string::npos) {
                outputFile = outputFile.substr(0, pos) + ".ir";
            } else {
                outputFile += ".ir";
            }
            
            // Salvar em arquivo
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << codegen.toString();
                outFile.close();
                if (verbose) {
                    std::cout << logSuccess("[INFO] Código intermediário salvo em: " + outputFile) << std::endl;
                }
            }
            
            delete ast;
        }
    }
    
    return result;
}

