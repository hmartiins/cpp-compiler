#include "semantic.h"

SemanticAnalyzer::SemanticAnalyzer() : currentFunction(nullptr) {}

void SemanticAnalyzer::collectParams(ASTNode* paramNode, std::vector<std::string>& params) {
    if (paramNode->symbol != PARAMS) return;
    
    if (paramNode->children.empty()) return; // sem parâmetros
    
    ASTNode* paramList = paramNode->children[0];
    
    // Percorrer recursivamente a lista de parâmetros
    while (paramList && paramList->symbol == PARAM_LIST) {
        if (!paramList->children.empty() && paramList->children[0]->symbol == T_ID) {
            params.push_back(paramList->children[0]->value);
        }
        
        // Próximo parâmetro
        if (paramList->children.size() > 1) {
            paramList = paramList->children[1];
        } else {
            break;
        }
    }
}

void SemanticAnalyzer::collectLocalVars(ASTNode* stmtNode) {
    if (!stmtNode || stmtNode->symbol != STATEMENT) return;
    
    // Se é atribuição, primeiro filho é ID
    if (!stmtNode->children.empty() && 
        stmtNode->children[0]->symbol == T_ID &&
        (stmtNode->children.size() > 1)) {
        // É uma atribuição
        if (stmtNode->children[0]->symbol == T_ID) {
            currentFunction->localVars.insert(stmtNode->children[0]->value);
        }
    }
}

int SemanticAnalyzer::countArguments(ASTNode* argsNode) {
    if (!argsNode || argsNode->symbol != ARGS) return 0;
    if (argsNode->children.empty()) return 0;
    
    int count = 0;
    ASTNode* argList = argsNode->children[0];
    
    while (argList && argList->symbol == ARG_LIST) {
        count++;
        
        if (argList->children.size() > 1) {
            argList = argList->children[1];
        } else {
            break;
        }
    }
    
    return count;
}

bool SemanticAnalyzer::checkIdentifier(const std::string& id) {
    if (!currentFunction) return false;
    
    // Verificar se é parâmetro
    for (const auto& param : currentFunction->params) {
        if (param == id) return true;
    }
    
    // Verificar se é variável local
    if (currentFunction->localVars.find(id) != currentFunction->localVars.end()) {
        return true;
    }
    
    return false;
}

bool SemanticAnalyzer::analyzeNode(ASTNode* node) {
    if (!node) return true;
    
    if (node->symbol == PROGRAM) {
        // Primeira passagem: coletar todas as funções
        if (!node->children.empty()) {
            ASTNode* funcList = node->children[0];
            while (funcList && funcList->symbol == FUNC_LIST) {
                if (!funcList->children.empty() && funcList->children[0]->symbol == FUNC_DECL) {
                    ASTNode* funcDecl = funcList->children[0];
                    if (funcDecl->children.size() >= 1 && funcDecl->children[0]->symbol == T_ID) {
                        std::string funcName = funcDecl->children[0]->value;
                        
                        // Verificar duplicação
                        if (functions.find(funcName) != functions.end()) {
                            errorMsg = "[ERROR] Erro semântico: função '" + funcName + "' já foi declarada";
                            return false;
                        }
                        
                        FunctionInfo info;
                        info.name = funcName;
                        
                        // Coletar parâmetros
                        if (funcDecl->children.size() >= 2) {
                            collectParams(funcDecl->children[1], info.params);
                        }
                        
                        functions[funcName] = info;
                    }
                }
                
                if (funcList->children.size() > 1) {
                    funcList = funcList->children[1];
                } else {
                    break;
                }
            }
        }
        
        // Segunda passagem: analisar cada função
        if (!node->children.empty()) {
            return analyzeNode(node->children[0]);
        }
    } else if (node->symbol == FUNC_LIST) {
        for (auto child : node->children) {
            if (!analyzeNode(child)) return false;
        }
    } else if (node->symbol == FUNC_DECL) {
        return analyzeFunctionDecl(node);
    }
    
    return true;
}

bool SemanticAnalyzer::analyzeFunctionDecl(ASTNode* node) {
    if (node->children.size() < 3) return false;
    
    std::string funcName = node->children[0]->value;
    currentFunction = &functions[funcName];
    
    // Coletar variáveis locais (primeira passagem nos statements)
    ASTNode* stmts = node->children[2];
    while (stmts && stmts->symbol == STATEMENTS) {
        if (!stmts->children.empty() && stmts->children[0]->symbol == STATEMENT) {
            collectLocalVars(stmts->children[0]);
        }
        
        if (stmts->children.size() > 1) {
            stmts = stmts->children[1];
        } else {
            break;
        }
    }
    
    // Analisar statements
    bool result = analyzeStatements(node->children[2]);
    
    currentFunction = nullptr;
    return result;
}

bool SemanticAnalyzer::analyzeStatements(ASTNode* node) {
    if (!node || node->symbol != STATEMENTS) return true;
    
    for (auto child : node->children) {
        if (child->symbol == STATEMENT) {
            if (!analyzeStatement(child)) return false;
        } else if (child->symbol == STATEMENTS) {
            if (!analyzeStatements(child)) return false;
        }
    }
    
    return true;
}

bool SemanticAnalyzer::analyzeStatement(ASTNode* node) {
    if (!node || node->symbol != STATEMENT) return true;
    
    // Analisar todas as expressões no statement
    for (auto child : node->children) {
        if (child->symbol == EXPR) {
            if (!analyzeExpr(child)) return false;
        }
    }
    
    return true;
}

bool SemanticAnalyzer::analyzeExpr(ASTNode* node) {
    if (!node) return true;
    
    // Se encontrar um ID, verificar se está declarado
    if (node->symbol == T_ID) {
        std::string idName = node->value;
        
        // Verificar se é uma função
        if (functions.find(idName) != functions.end()) {
            return true; // É uma função declarada
        }
        
        // Verificar se é variável ou parâmetro
        if (!checkIdentifier(idName)) {
            errorMsg = "[ERROR] Erro semântico: identificador '" + idName + "' não foi declarado";
            return false;
        }
        
        return true;
    }
    
    // Se for BASE com chamada de função
    if (node->symbol == BASE && node->children.size() >= 2) {
        if (node->children[0]->symbol == T_ID && node->children[1]->symbol == ARGS) {
            std::string funcName = node->children[0]->value;
            
            // Verificar se função existe
            if (functions.find(funcName) == functions.end()) {
                errorMsg = "[ERROR] Erro semântico: função '" + funcName + "' não foi declarada";
                return false;
            }
            
            // Verificar número de argumentos
            int argsCount = countArguments(node->children[1]);
            int expectedCount = functions[funcName].params.size();
            
            if (argsCount != expectedCount) {
                errorMsg = "[ERROR] Erro semântico: função '" + funcName + "' espera " + 
                          std::to_string(expectedCount) + " argumento(s), mas " + 
                          std::to_string(argsCount) + " foi(ram) fornecido(s)";
                return false;
            }
        }
    }
    
    // Analisar recursivamente os filhos
    for (auto child : node->children) {
        if (!analyzeExpr(child)) return false;
    }
    
    return true;
}

bool SemanticAnalyzer::analyze(ASTNode* root) {
    errorMsg = "";
    functions.clear();
    currentFunction = nullptr;
    
    return analyzeNode(root);
}

std::string SemanticAnalyzer::getError() const {
    return errorMsg;
}

