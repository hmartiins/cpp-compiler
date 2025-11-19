#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "ast.h"
#include <string>
#include <vector>
#include <map>
#include <set>

struct FunctionInfo {
    std::string name;
    std::vector<std::string> params;
    std::set<std::string> localVars;
};

class SemanticAnalyzer {
private:
    std::map<std::string, FunctionInfo> functions;
    FunctionInfo* currentFunction;
    std::string errorMsg;
    
    bool analyzeNode(ASTNode* node);
    bool analyzeFunctionDecl(ASTNode* node);
    bool analyzeStatements(ASTNode* node);
    bool analyzeStatement(ASTNode* node);
    bool analyzeExpr(ASTNode* node);
    
    void collectParams(ASTNode* paramNode, std::vector<std::string>& params);
    bool checkIdentifier(const std::string& id);
    void collectLocalVars(ASTNode* stmtNode);
    int countArguments(ASTNode* argsNode);
    
public:
    SemanticAnalyzer();
    bool analyze(ASTNode* root);
    std::string getError() const;
};

#endif // SEMANTIC_H

