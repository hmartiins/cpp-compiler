#include "codegen.h"
#include <sstream>
#include <iostream>

CodeGenerator::CodeGenerator() : tempCounter(0), currentFunction("") {}

std::string CodeGenerator::newTemp() {
    return "t" + std::to_string(tempCounter++);
}

std::string CodeGenerator::generateExpr(ASTNode* node) {
    if (!node || node->symbol != EXPR) {
        return "";
    }
    
    // EXPR -> TERM EXPR_P
    if (node->children.size() < 2) return "";
    
    ASTNode* term = node->children[0];
    ASTNode* exprP = node->children[1];
    
    std::string result = generateTerm(term);
    
    // Processar EXPR_P (pode ter mais operações)
    while (exprP && exprP->symbol == EXPR_P && exprP->children.size() > 0) {
        if (exprP->children.size() >= 3) {
            // Operador encontrado
            std::string op = "";
            if (exprP->children[0]->symbol == T_PLUS) op = "+";
            else if (exprP->children[0]->symbol == T_MINUS) op = "-";
            
            if (!op.empty()) {
                std::string termResult = generateTerm(exprP->children[1]);
                std::string temp = newTemp();
                code.push_back(ThreeAddressCode(temp, op, result, termResult));
                result = temp;
            }
            exprP = exprP->children.size() > 2 ? exprP->children[2] : nullptr;
        } else {
            break;
        }
    }
    
    return result;
}

std::string CodeGenerator::generateTerm(ASTNode* node) {
    if (!node || node->symbol != TERM) {
        return "";
    }
    
    if (node->children.size() < 2) return "";
    
    ASTNode* factor = node->children[0];
    ASTNode* termP = node->children[1];
    
    std::string result = generateFactor(factor);
    
    // Processar TERM_P
    while (termP && termP->symbol == TERM_P && termP->children.size() > 0) {
        if (termP->children.size() >= 3) {
            std::string op = "";
            if (termP->children[0]->symbol == T_MULT) op = "*";
            else if (termP->children[0]->symbol == T_DIV) op = "/";
            
            if (!op.empty()) {
                std::string factorResult = generateFactor(termP->children[1]);
                std::string temp = newTemp();
                code.push_back(ThreeAddressCode(temp, op, result, factorResult));
                result = temp;
            }
            termP = termP->children.size() > 2 ? termP->children[2] : nullptr;
        } else {
            break;
        }
    }
    
    return result;
}

std::string CodeGenerator::generateFactor(ASTNode* node) {
    if (!node || node->symbol != FACTOR) {
        return "";
    }
    
    if (node->children.size() < 2) return "";
    
    ASTNode* base = node->children[0];
    ASTNode* factorP = node->children[1];
    
    std::string result = generateBase(base);
    
    // Processar FACTOR_P (potência)
    while (factorP && factorP->symbol == FACTOR_P && factorP->children.size() > 0) {
        if (factorP->children.size() >= 3 && factorP->children[0]->symbol == T_POW) {
            std::string baseResult = generateBase(factorP->children[1]);
            std::string temp = newTemp();
            code.push_back(ThreeAddressCode(temp, "^", result, baseResult));
            result = temp;
            factorP = factorP->children.size() > 2 ? factorP->children[2] : nullptr;
        } else {
            break;
        }
    }
    
    return result;
}

std::string CodeGenerator::generateBase(ASTNode* node) {
    if (!node || node->symbol != BASE) {
        return "";
    }
    
    if (node->children.empty()) return "";
    
    ASTNode* firstChild = node->children[0];
    
    // NUM
    if (firstChild->symbol == T_NUM) {
        return firstChild->value;
    }
    
    // ID (variável)
    if (firstChild->symbol == T_ID) {
        // Verificar se é chamada de função
        if (node->children.size() > 1 && node->children[1]->symbol == ARGS) {
            std::string funcName = firstChild->value;
            ASTNode* args = node->children[1];
            
            // Coletar argumentos
            std::vector<std::string> argList;
            if (!args->children.empty()) {
                ASTNode* argListNode = args->children[0];
                while (argListNode && argListNode->symbol == ARG_LIST) {
                    if (!argListNode->children.empty()) {
                        std::string arg = generateExpr(argListNode->children[0]);
                        argList.push_back(arg);
                    }
                    if (argListNode->children.size() > 1) {
                        argListNode = argListNode->children[1];
                    } else {
                        break;
                    }
                }
            }
            
            // Gerar chamada de função
            std::string temp = newTemp();
            std::string argsStr = "";
            for (size_t i = 0; i < argList.size(); i++) {
                if (i > 0) argsStr += ", ";
                argsStr += argList[i];
            }
            code.push_back(ThreeAddressCode(temp, "CALL", funcName, argsStr));
            return temp;
        }
        
        // Apenas variável
        return firstChild->value;
    }
    
    // EXPR entre parênteses
    if (firstChild->symbol == EXPR) {
        return generateExpr(firstChild);
    }
    
    return "";
}

void CodeGenerator::generateStatement(ASTNode* node) {
    if (!node || node->symbol != STATEMENT) {
        return;
    }
    
    if (node->children.empty()) return;
    
    ASTNode* firstChild = node->children[0];
    
    // RETURN
    if (firstChild->symbol == T_RETURN && node->children.size() > 1) {
        std::string exprResult = generateExpr(node->children[1]);
        code.push_back(ThreeAddressCode("", "RETURN", exprResult));
        return;
    }
    
    // Atribuição: ID ASSIGN EXPR
    if (firstChild->symbol == T_ID && node->children.size() > 1) {
        std::string varName = firstChild->value;
        std::string exprResult = generateExpr(node->children[1]);
        code.push_back(ThreeAddressCode(varName, "=", exprResult));
        return;
    }
}

void CodeGenerator::generateStatements(ASTNode* node) {
    if (!node || node->symbol != STATEMENTS) {
        return;
    }
    
    for (auto child : node->children) {
        if (child->symbol == STATEMENT) {
            generateStatement(child);
        } else if (child->symbol == STATEMENTS) {
            generateStatements(child);
        }
    }
}

void CodeGenerator::generateFunction(ASTNode* node) {
    if (!node || node->symbol != FUNC_DECL) {
        return;
    }
    
    if (node->children.empty()) return;
    
    // Primeiro filho é o nome da função
    if (node->children[0]->symbol == T_ID) {
        currentFunction = node->children[0]->value;
        code.push_back(ThreeAddressCode("", "FUNC", currentFunction));
        
        // Gerar statements
        if (node->children.size() > 2) {
            generateStatements(node->children[2]);
        }
        
        code.push_back(ThreeAddressCode("", "ENDFUNC", currentFunction));
        currentFunction = "";
    }
}

std::vector<ThreeAddressCode> CodeGenerator::generate(ASTNode* root) {
    code.clear();
    tempCounter = 0;
    
    if (!root || root->symbol != PROGRAM) {
        return code;
    }
    
    // PROCESSAR FUNC_LIST
    if (!root->children.empty()) {
        ASTNode* funcList = root->children[0];
        while (funcList && funcList->symbol == FUNC_LIST) {
            if (!funcList->children.empty() && funcList->children[0]->symbol == FUNC_DECL) {
                generateFunction(funcList->children[0]);
            }
            if (funcList->children.size() > 1) {
                funcList = funcList->children[1];
            } else {
                break;
            }
        }
    }
    
    return code;
}

std::string CodeGenerator::toString() const {
    std::ostringstream oss;
    
    for (const auto& instr : code) {
        if (instr.op == "FUNC") {
            oss << "\n=== Funcao: " << instr.arg1 << " ===\n";
        } else if (instr.op == "ENDFUNC") {
            oss << "=== Fim: " << instr.arg1 << " ===\n";
        } else if (instr.op == "RETURN") {
            oss << "  RETURN " << instr.arg1 << "\n";
        } else if (instr.op == "CALL") {
            oss << "  " << instr.result << " = CALL " << instr.arg1 << "(" << instr.arg2 << ")\n";
        } else if (instr.op == "=") {
            oss << "  " << instr.result << " = " << instr.arg1 << "\n";
        } else if (!instr.arg2.empty()) {
            oss << "  " << instr.result << " = " << instr.arg1 << " " << instr.op << " " << instr.arg2 << "\n";
        } else {
            oss << "  " << instr.result << " = " << instr.op << " " << instr.arg1 << "\n";
        }
    }
    
    return oss.str();
}
