#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include <string>
#include <vector>

// Instrução de código de três endereços
struct ThreeAddressCode {
    std::string result;      // Variável temporária ou variável de resultado
    std::string op;          // Operador (+, -, *, /, ^, =, CALL, RETURN)
    std::string arg1;        // Primeiro operando
    std::string arg2;        // Segundo operando (pode ser vazio)
    
    ThreeAddressCode(const std::string& r, const std::string& o, 
                    const std::string& a1, const std::string& a2 = "")
        : result(r), op(o), arg1(a1), arg2(a2) {}
};

class CodeGenerator {
private:
    std::vector<ThreeAddressCode> code;
    int tempCounter;
    std::string currentFunction;
    
    std::string newTemp();
    std::string generateExpr(ASTNode* node);
    std::string generateTerm(ASTNode* node);
    std::string generateFactor(ASTNode* node);
    std::string generateBase(ASTNode* node);
    void generateStatement(ASTNode* node);
    void generateStatements(ASTNode* node);
    void generateFunction(ASTNode* node);
    
public:
    CodeGenerator();
    std::vector<ThreeAddressCode> generate(ASTNode* root);
    std::string toString() const;
};

#endif // CODEGEN_H

