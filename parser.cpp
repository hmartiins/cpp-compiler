#include "parser.h"

Parser::Parser(const std::vector<Token>& toks) : tokens(toks), pos(0) {
    if (!tokens.empty()) {
        currentToken = tokens[0];
    }
}

void Parser::advance() {
    pos++;
    if (pos < tokens.size()) {
        currentToken = tokens[pos];
    }
}

bool Parser::match(TokenType type) {
    return currentToken.type == type;
}

bool Parser::expect(TokenType type, const std::string& msg) {
    if (match(type)) {
        advance();
        return true;
    }
    errorMsg = "[ERROR] Erro sintático na linha " + std::to_string(currentToken.line) + 
               ", coluna " + std::to_string(currentToken.column) + ": " + msg +
               ". Encontrado: " + currentToken.lexeme;
    return false;
}

ASTNode* Parser::parse() {
    pos = 0;
    if (!tokens.empty()) {
        currentToken = tokens[0];
    }
    errorMsg = "";
    
    ASTNode* root = parseProgram();
    
    if (!root) {
        return nullptr;
    }
    
    if (!match(END_OF_FILE)) {
        errorMsg = "[ERROR] Erro sintático: esperado fim de arquivo na linha " + 
                   std::to_string(currentToken.line);
        delete root;
        return nullptr;
    }
    
    return root;
}

std::string Parser::getError() const {
    return errorMsg;
}

ASTNode* Parser::parseProgram() {
    ASTNode* node = new ASTNode(PROGRAM);
    ASTNode* funcList = parseFunctionList();
    if (!funcList) {
        delete node;
        return nullptr;
    }
    node->addChild(funcList);
    return node;
}

ASTNode* Parser::parseFunctionList() {
    ASTNode* node = new ASTNode(FUNC_LIST);
    
    if (match(FUNCTION)) {
        ASTNode* funcDecl = parseFunctionDecl();
        if (!funcDecl) {
            delete node;
            return nullptr;
        }
        node->addChild(funcDecl);
        
        ASTNode* nextFuncList = parseFunctionList();
        if (!nextFuncList) {
            delete node;
            return nullptr;
        }
        node->addChild(nextFuncList);
    }
    // else: epsilon (lista vazia)
    
    return node;
}

ASTNode* Parser::parseFunctionDecl() {
    ASTNode* node = new ASTNode(FUNC_DECL);
    
    if (!expect(FUNCTION, "esperado 'func'")) {
        delete node;
        return nullptr;
    }
    
    if (!match(ID)) {
        errorMsg = "[ERROR] Erro sintático: esperado nome da função na linha " + 
                   std::to_string(currentToken.line);
        delete node;
        return nullptr;
    }
    ASTNode* funcName = new ASTNode(T_ID, currentToken.lexeme);
    node->addChild(funcName);
    advance();
    
    if (!expect(LPAREN, "esperado '('")) {
        delete node;
        return nullptr;
    }
    
    ASTNode* params = parseParams();
    if (!params) {
        delete node;
        return nullptr;
    }
    node->addChild(params);
    
    if (!expect(RPAREN, "esperado ')'")) {
        delete node;
        return nullptr;
    }
    
    if (!expect(LBRACE, "esperado '{'")) {
        delete node;
        return nullptr;
    }
    
    ASTNode* stmts = parseStatements();
    if (!stmts) {
        delete node;
        return nullptr;
    }
    node->addChild(stmts);
    
    if (!expect(RBRACE, "esperado '}'")) {
        delete node;
        return nullptr;
    }
    
    return node;
}

ASTNode* Parser::parseParams() {
    ASTNode* node = new ASTNode(PARAMS);
    
    if (match(ID)) {
        ASTNode* paramList = parseParamList();
        if (!paramList) {
            delete node;
            return nullptr;
        }
        node->addChild(paramList);
    }
    // else: epsilon (sem parâmetros)
    
    return node;
}

ASTNode* Parser::parseParamList() {
    ASTNode* node = new ASTNode(PARAM_LIST);
    
    if (!match(ID)) {
        errorMsg = "[ERROR] Erro sintático: esperado identificador de parâmetro na linha " + 
                   std::to_string(currentToken.line);
        delete node;
        return nullptr;
    }
    ASTNode* param = new ASTNode(T_ID, currentToken.lexeme);
    node->addChild(param);
    advance();
    
    if (match(COMMA)) {
        advance();
        ASTNode* nextParams = parseParamList();
        if (!nextParams) {
            delete node;
            return nullptr;
        }
        node->addChild(nextParams);
    }
    
    return node;
}

ASTNode* Parser::parseStatements() {
    ASTNode* node = new ASTNode(STATEMENTS);
    
    if (match(ID) || match(RETURN)) {
        ASTNode* stmt = parseStatement();
        if (!stmt) {
            delete node;
            return nullptr;
        }
        node->addChild(stmt);
        
        ASTNode* nextStmts = parseStatements();
        if (!nextStmts) {
            delete node;
            return nullptr;
        }
        node->addChild(nextStmts);
    }
    // else: epsilon (sem mais statements)
    
    return node;
}

ASTNode* Parser::parseStatement() {
    ASTNode* node = new ASTNode(STATEMENT);
    
    if (match(RETURN)) {
        advance();
        ASTNode* expr = parseExpr();
        if (!expr) {
            delete node;
            return nullptr;
        }
        node->addChild(new ASTNode(T_RETURN, "return"));
        node->addChild(expr);
        
        if (!expect(SEMICOLON, "esperado ';'")) {
            delete node;
            return nullptr;
        }
    } else if (match(ID)) {
        std::string varName = currentToken.lexeme;
        ASTNode* idNode = new ASTNode(T_ID, varName);
        node->addChild(idNode);
        advance();
        
        if (!expect(ASSIGN, "esperado '='")) {
            delete node;
            return nullptr;
        }
        
        ASTNode* expr = parseExpr();
        if (!expr) {
            delete node;
            return nullptr;
        }
        node->addChild(expr);
        
        if (!expect(SEMICOLON, "esperado ';'")) {
            delete node;
            return nullptr;
        }
    } else {
        errorMsg = "[ERROR] Erro sintático: esperado statement na linha " + 
                   std::to_string(currentToken.line);
        delete node;
        return nullptr;
    }
    
    return node;
}

ASTNode* Parser::parseExpr() {
    ASTNode* node = new ASTNode(EXPR);
    
    ASTNode* term = parseTerm();
    if (!term) {
        delete node;
        return nullptr;
    }
    node->addChild(term);
    
    ASTNode* exprP = parseExprP();
    if (!exprP) {
        delete node;
        return nullptr;
    }
    node->addChild(exprP);
    
    return node;
}

ASTNode* Parser::parseExprP() {
    ASTNode* node = new ASTNode(EXPR_P);
    
    if (match(PLUS) || match(MINUS)) {
        TokenType op = currentToken.type;
        advance();
        node->addChild(new ASTNode(op == PLUS ? T_PLUS : T_MINUS, op == PLUS ? "+" : "-"));
        
        ASTNode* term = parseTerm();
        if (!term) {
            delete node;
            return nullptr;
        }
        node->addChild(term);
        
        ASTNode* exprP = parseExprP();
        if (!exprP) {
            delete node;
            return nullptr;
        }
        node->addChild(exprP);
    }
    // else: epsilon
    
    return node;
}

ASTNode* Parser::parseTerm() {
    ASTNode* node = new ASTNode(TERM);
    
    ASTNode* factor = parseFactor();
    if (!factor) {
        delete node;
        return nullptr;
    }
    node->addChild(factor);
    
    ASTNode* termP = parseTermP();
    if (!termP) {
        delete node;
        return nullptr;
    }
    node->addChild(termP);
    
    return node;
}

ASTNode* Parser::parseTermP() {
    ASTNode* node = new ASTNode(TERM_P);
    
    if (match(MULT) || match(DIV)) {
        TokenType op = currentToken.type;
        advance();
        node->addChild(new ASTNode(op == MULT ? T_MULT : T_DIV, op == MULT ? "*" : "/"));
        
        ASTNode* factor = parseFactor();
        if (!factor) {
            delete node;
            return nullptr;
        }
        node->addChild(factor);
        
        ASTNode* termP = parseTermP();
        if (!termP) {
            delete node;
            return nullptr;
        }
        node->addChild(termP);
    }
    // else: epsilon
    
    return node;
}

ASTNode* Parser::parseFactor() {
    ASTNode* node = new ASTNode(FACTOR);
    
    ASTNode* base = parseBase();
    if (!base) {
        delete node;
        return nullptr;
    }
    node->addChild(base);
    
    ASTNode* factorP = parseFactorP();
    if (!factorP) {
        delete node;
        return nullptr;
    }
    node->addChild(factorP);
    
    return node;
}

ASTNode* Parser::parseFactorP() {
    ASTNode* node = new ASTNode(FACTOR_P);
    
    if (match(POW)) {
        advance();
        node->addChild(new ASTNode(T_POW, "^"));
        
        ASTNode* base = parseBase();
        if (!base) {
            delete node;
            return nullptr;
        }
        node->addChild(base);
        
        ASTNode* factorP = parseFactorP();
        if (!factorP) {
            delete node;
            return nullptr;
        }
        node->addChild(factorP);
    }
    // else: epsilon
    
    return node;
}

ASTNode* Parser::parseBase() {
    ASTNode* node = new ASTNode(BASE);
    
    if (match(NUM)) {
        node->addChild(new ASTNode(T_NUM, currentToken.lexeme));
        advance();
    } else if (match(ID)) {
        std::string idName = currentToken.lexeme;
        advance();
        
        if (match(LPAREN)) {
            // Chamada de função
            node->addChild(new ASTNode(T_ID, idName));
            advance();
            
            ASTNode* args = parseArgs();
            if (!args) {
                delete node;
                return nullptr;
            }
            node->addChild(args);
            
            if (!expect(RPAREN, "esperado ')'")) {
                delete node;
                return nullptr;
            }
        } else {
            // Apenas variável
            node->addChild(new ASTNode(T_ID, idName));
        }
    } else if (match(LPAREN)) {
        advance();
        
        ASTNode* expr = parseExpr();
        if (!expr) {
            delete node;
            return nullptr;
        }
        node->addChild(expr);
        
        if (!expect(RPAREN, "esperado ')'")) {
            delete node;
            return nullptr;
        }
    } else {
        errorMsg = "[ERROR] Erro sintático: esperado expressão na linha " + 
                   std::to_string(currentToken.line);
        delete node;
        return nullptr;
    }
    
    return node;
}

ASTNode* Parser::parseArgs() {
    ASTNode* node = new ASTNode(ARGS);
    
    // Verificar se há argumentos (lookahead)
    if (!match(RPAREN)) {
        ASTNode* argList = parseArgList();
        if (!argList) {
            delete node;
            return nullptr;
        }
        node->addChild(argList);
    }
    // else: epsilon (sem argumentos)
    
    return node;
}

ASTNode* Parser::parseArgList() {
    ASTNode* node = new ASTNode(ARG_LIST);
    
    ASTNode* expr = parseExpr();
    if (!expr) {
        delete node;
        return nullptr;
    }
    node->addChild(expr);
    
    if (match(COMMA)) {
        advance();
        ASTNode* nextArgs = parseArgList();
        if (!nextArgs) {
            delete node;
            return nullptr;
        }
        node->addChild(nextArgs);
    }
    
    return node;
}

