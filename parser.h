#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "ast.h"
#include <vector>
#include <string>

// Parser Recursivo Descendente (simplificado como LR)
// Para um projeto acadêmico completo com LR real, seria necessário gerar as tabelas
// Aqui implementamos um parser recursivo que simula comportamento LR
class Parser {
private:
    std::vector<Token> tokens;
    size_t pos;
    Token currentToken;
    std::string errorMsg;
    
    void advance();
    bool match(TokenType type);
    bool expect(TokenType type, const std::string& msg);
    
    // Regras de produção
    ASTNode* parseProgram();
    ASTNode* parseFunctionList();
    ASTNode* parseFunctionDecl();
    ASTNode* parseParams();
    ASTNode* parseParamList();
    ASTNode* parseStatements();
    ASTNode* parseStatement();
    ASTNode* parseExpr();
    ASTNode* parseExprP();
    ASTNode* parseTerm();
    ASTNode* parseTermP();
    ASTNode* parseFactor();
    ASTNode* parseFactorP();
    ASTNode* parseBase();
    ASTNode* parseArgs();
    ASTNode* parseArgList();
    
public:
    Parser(const std::vector<Token>& toks);
    ASTNode* parse();
    std::string getError() const;
};

#endif // PARSER_H

