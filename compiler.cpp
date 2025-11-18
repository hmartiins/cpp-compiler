#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <stack>
#include <cctype>
#include <sstream>
#include <fstream>

using namespace std;

// ==================== ANALISADOR LÉXICO ====================

enum TokenType {
    // Palavras-chave
    FUNCTION, RETURN,
    // Identificadores e literais
    ID, NUM,
    // Operadores
    PLUS, MINUS, MULT, DIV, POW, ASSIGN,
    // Delimitadores
    LPAREN, RPAREN, LBRACE, RBRACE, COMMA, SEMICOLON,
    // Especiais
    END_OF_FILE, UNKNOWN
};

struct Token {
    TokenType type;
    string lexeme;
    int line;
    int column;
    
    Token(TokenType t = UNKNOWN, string lex = "", int l = 1, int c = 1) 
        : type(t), lexeme(lex), line(l), column(c) {}
};

string tokenTypeToString(TokenType type) {
    switch(type) {
        case FUNCTION: return "FUNCTION";
        case RETURN: return "RETURN";
        case ID: return "ID";
        case NUM: return "NUM";
        case PLUS: return "PLUS";
        case MINUS: return "MINUS";
        case MULT: return "MULT";
        case DIV: return "DIV";
        case POW: return "POW";
        case ASSIGN: return "ASSIGN";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case COMMA: return "COMMA";
        case SEMICOLON: return "SEMICOLON";
        case END_OF_FILE: return "EOF";
        default: return "UNKNOWN";
    }
}

class Lexer {
private:
    string input;
    size_t pos;
    int line;
    int column;
    map<string, TokenType> keywords;
    
    char peek(int offset = 0) {
        if (pos + offset >= input.length()) return '\0';
        return input[pos + offset];
    }
    
    char advance() {
        if (pos >= input.length()) return '\0';
        char ch = input[pos++];
        if (ch == '\n') {
            line++;
            column = 1;
        } else {
            column++;
        }
        return ch;
    }
    
    void skipWhitespace() {
        while (isspace(peek())) {
            advance();
        }
    }
    
    void skipComment() {
        if (peek() == '/' && peek(1) == '/') {
            // Comentário de linha
            while (peek() != '\n' && peek() != '\0') {
                advance();
            }
        } else if (peek() == '/' && peek(1) == '*') {
            // Comentário de bloco
            advance(); // /
            advance(); // *
            while (true) {
                if (peek() == '\0') break;
                if (peek() == '*' && peek(1) == '/') {
                    advance(); // *
                    advance(); // /
                    break;
                }
                advance();
            }
        }
    }
    
    Token readNumber() {
        int startCol = column;
        string num;
        bool hasDecimal = false;
        
        while (isdigit(peek()) || peek() == '.') {
            if (peek() == '.') {
                if (hasDecimal) break; // Segundo ponto, para
                hasDecimal = true;
            }
            num += advance();
        }
        
        return Token(NUM, num, line, startCol);
    }
    
    Token readIdentifier() {
        int startCol = column;
        string id;
        
        while (isalnum(peek()) || peek() == '_') {
            id += advance();
        }
        
        // Verificar se é palavra-chave
        if (keywords.find(id) != keywords.end()) {
            return Token(keywords[id], id, line, startCol);
        }
        
        return Token(ID, id, line, startCol);
    }
    
public:
    Lexer(const string& source) : input(source), pos(0), line(1), column(1) {
        // Inicializar palavras-chave
        keywords["function"] = FUNCTION;
        keywords["return"] = RETURN;
    }
    
    Token nextToken() {
        skipWhitespace();
        
        // Pular comentários
        while ((peek() == '/' && peek(1) == '/') || (peek() == '/' && peek(1) == '*')) {
            skipComment();
            skipWhitespace();
        }
        
        if (peek() == '\0') {
            return Token(END_OF_FILE, "", line, column);
        }
        
        int startCol = column;
        char ch = peek();
        
        // Números
        if (isdigit(ch)) {
            return readNumber();
        }
        
        // Identificadores e palavras-chave
        if (isalpha(ch) || ch == '_') {
            return readIdentifier();
        }
        
        // Operadores e delimitadores
        advance();
        switch (ch) {
            case '+': return Token(PLUS, "+", line, startCol);
            case '-': return Token(MINUS, "-", line, startCol);
            case '*': return Token(MULT, "*", line, startCol);
            case '/': return Token(DIV, "/", line, startCol);
            case '^': return Token(POW, "^", line, startCol);
            case '=': return Token(ASSIGN, "=", line, startCol);
            case '(': return Token(LPAREN, "(", line, startCol);
            case ')': return Token(RPAREN, ")", line, startCol);
            case '{': return Token(LBRACE, "{", line, startCol);
            case '}': return Token(RBRACE, "}", line, startCol);
            case ',': return Token(COMMA, ",", line, startCol);
            case ';': return Token(SEMICOLON, ";", line, startCol);
            default:
                return Token(UNKNOWN, string(1, ch), line, startCol);
        }
    }
    
    vector<Token> tokenize() {
        vector<Token> tokens;
        Token tok;
        do {
            tok = nextToken();
            tokens.push_back(tok);
        } while (tok.type != END_OF_FILE);
        return tokens;
    }
};

// ==================== ANALISADOR SINTÁTICO (Parser LR) ====================

// Símbolos da gramática
enum Symbol {
    // Não-terminais
    PROGRAM, FUNC_LIST, FUNC_DECL, PARAMS, PARAM_LIST, 
    STATEMENTS, STATEMENT, EXPR, EXPR_P, TERM, TERM_P, 
    FACTOR, FACTOR_P, BASE, ARGS, ARG_LIST,
    // Terminais (correspondem aos tokens)
    T_FUNCTION, T_RETURN, T_ID, T_NUM, T_PLUS, T_MINUS, 
    T_MULT, T_DIV, T_POW, T_ASSIGN, T_LPAREN, T_RPAREN, 
    T_LBRACE, T_RBRACE, T_COMMA, T_SEMICOLON, T_EOF, T_EPSILON
};

string symbolToString(Symbol s) {
    switch(s) {
        case PROGRAM: return "program";
        case FUNC_LIST: return "function_list";
        case FUNC_DECL: return "function_decl";
        case PARAMS: return "params";
        case PARAM_LIST: return "param_list";
        case STATEMENTS: return "statements";
        case STATEMENT: return "statement";
        case EXPR: return "expr";
        case EXPR_P: return "expr'";
        case TERM: return "term";
        case TERM_P: return "term'";
        case FACTOR: return "factor";
        case FACTOR_P: return "factor'";
        case BASE: return "base";
        case ARGS: return "args";
        case ARG_LIST: return "arg_list";
        case T_FUNCTION: return "function";
        case T_RETURN: return "return";
        case T_ID: return "ID";
        case T_NUM: return "NUM";
        case T_PLUS: return "+";
        case T_MINUS: return "-";
        case T_MULT: return "*";
        case T_DIV: return "/";
        case T_POW: return "^";
        case T_ASSIGN: return "=";
        case T_LPAREN: return "(";
        case T_RPAREN: return ")";
        case T_LBRACE: return "{";
        case T_RBRACE: return "}";
        case T_COMMA: return ",";
        case T_SEMICOLON: return ";";
        case T_EOF: return "$";
        case T_EPSILON: return "ε";
        default: return "?";
    }
}

Symbol tokenTypeToSymbol(TokenType type) {
    switch(type) {
        case FUNCTION: return T_FUNCTION;
        case RETURN: return T_RETURN;
        case ID: return T_ID;
        case NUM: return T_NUM;
        case PLUS: return T_PLUS;
        case MINUS: return T_MINUS;
        case MULT: return T_MULT;
        case DIV: return T_DIV;
        case POW: return T_POW;
        case ASSIGN: return T_ASSIGN;
        case LPAREN: return T_LPAREN;
        case RPAREN: return T_RPAREN;
        case LBRACE: return T_LBRACE;
        case RBRACE: return T_RBRACE;
        case COMMA: return T_COMMA;
        case SEMICOLON: return T_SEMICOLON;
        case END_OF_FILE: return T_EOF;
        default: return T_EOF;
    }
}

// Árvore sintática abstrata (AST)
struct ASTNode {
    Symbol symbol;
    string value;
    vector<ASTNode*> children;
    
    ASTNode(Symbol s, string v = "") : symbol(s), value(v) {}
    
    ~ASTNode() {
        for (auto child : children) {
            delete child;
        }
    }
    
    void addChild(ASTNode* child) {
        children.push_back(child);
    }
};

// Parser Recursivo Descendente (simplificado como LR)
// Para um projeto acadêmico completo com LR real, seria necessário gerar as tabelas
// Aqui implementamos um parser recursivo que simula comportamento LR
class Parser {
private:
    vector<Token> tokens;
    size_t pos;
    Token currentToken;
    string errorMsg;
    
    void advance() {
        pos++;
        if (pos < tokens.size()) {
            currentToken = tokens[pos];
        }
    }
    
    bool match(TokenType type) {
        return currentToken.type == type;
    }
    
    bool expect(TokenType type, const string& msg) {
        if (match(type)) {
            advance();
            return true;
        }
        errorMsg = "Erro sintático na linha " + to_string(currentToken.line) + 
                   ", coluna " + to_string(currentToken.column) + ": " + msg +
                   ". Encontrado: " + currentToken.lexeme;
        return false;
    }
    
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
    Parser(const vector<Token>& toks) : tokens(toks), pos(0) {
        if (!tokens.empty()) {
            currentToken = tokens[0];
        }
    }
    
    ASTNode* parse() {
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
            errorMsg = "Erro sintático: esperado fim de arquivo na linha " + 
                       to_string(currentToken.line);
            delete root;
            return nullptr;
        }
        
        return root;
    }
    
    string getError() const { return errorMsg; }
};

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
    
    if (!expect(FUNCTION, "esperado 'function'")) {
        delete node;
        return nullptr;
    }
    
    if (!match(ID)) {
        errorMsg = "Erro sintático: esperado nome da função na linha " + 
                   to_string(currentToken.line);
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
        errorMsg = "Erro sintático: esperado identificador de parâmetro na linha " + 
                   to_string(currentToken.line);
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
        string varName = currentToken.lexeme;
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
        errorMsg = "Erro sintático: esperado statement na linha " + 
                   to_string(currentToken.line);
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
        string idName = currentToken.lexeme;
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
        errorMsg = "Erro sintático: esperado expressão na linha " + 
                   to_string(currentToken.line);
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

// ==================== ANALISADOR SEMÂNTICO ====================

struct FunctionInfo {
    string name;
    vector<string> params;
    set<string> localVars;
};

class SemanticAnalyzer {
private:
    map<string, FunctionInfo> functions;
    FunctionInfo* currentFunction;
    string errorMsg;
    
    bool analyzeNode(ASTNode* node);
    bool analyzeFunctionDecl(ASTNode* node);
    bool analyzeStatements(ASTNode* node);
    bool analyzeStatement(ASTNode* node);
    bool analyzeExpr(ASTNode* node);
    
    void collectParams(ASTNode* paramNode, vector<string>& params);
    bool checkIdentifier(const string& id);
    void collectLocalVars(ASTNode* stmtNode);
    int countArguments(ASTNode* argsNode);
    
public:
    SemanticAnalyzer() : currentFunction(nullptr) {}
    
    bool analyze(ASTNode* root) {
        errorMsg = "";
        functions.clear();
        currentFunction = nullptr;
        
        return analyzeNode(root);
    }
    
    string getError() const { return errorMsg; }
};

void SemanticAnalyzer::collectParams(ASTNode* paramNode, vector<string>& params) {
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

bool SemanticAnalyzer::checkIdentifier(const string& id) {
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
                        string funcName = funcDecl->children[0]->value;
                        
                        // Verificar duplicação
                        if (functions.find(funcName) != functions.end()) {
                            errorMsg = "Erro semântico: função '" + funcName + "' já foi declarada";
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
    
    string funcName = node->children[0]->value;
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
        string idName = node->value;
        
        // Verificar se é uma função
        if (functions.find(idName) != functions.end()) {
            return true; // É uma função declarada
        }
        
        // Verificar se é variável ou parâmetro
        if (!checkIdentifier(idName)) {
            errorMsg = "Erro semântico: identificador '" + idName + "' não foi declarado";
            return false;
        }
        
        return true;
    }
    
    // Se for BASE com chamada de função
    if (node->symbol == BASE && node->children.size() >= 2) {
        if (node->children[0]->symbol == T_ID && node->children[1]->symbol == ARGS) {
            string funcName = node->children[0]->value;
            
            // Verificar se função existe
            if (functions.find(funcName) == functions.end()) {
                errorMsg = "Erro semântico: função '" + funcName + "' não foi declarada";
                return false;
            }
            
            // Verificar número de argumentos
            int argsCount = countArguments(node->children[1]);
            int expectedCount = functions[funcName].params.size();
            
            if (argsCount != expectedCount) {
                errorMsg = "Erro semântico: função '" + funcName + "' espera " + 
                          to_string(expectedCount) + " argumento(s), mas " + 
                          to_string(argsCount) + " foi(ram) fornecido(s)";
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

// ==================== COMPILADOR PRINCIPAL ====================

class Compiler {
private:
    string sourceCode;
    bool verbose;
    
public:
    Compiler(bool v = false) : verbose(v) {}
    
    bool compile(const string& source) {
        sourceCode = source;
        
        // Fase 1: Análise Léxica
        if (verbose) cout << "=== ANÁLISE LÉXICA ===" << endl;
        Lexer lexer(sourceCode);
        vector<Token> tokens = lexer.tokenize();
        
        // Verificar erros léxicos
        for (const auto& tok : tokens) {
            if (tok.type == UNKNOWN) {
                cout << "REJEITADO" << endl;
                cout << "Erro léxico na linha " << tok.line << ", coluna " << tok.column 
                     << ": caractere inválido '" << tok.lexeme << "'" << endl;
                return false;
            }
        }
        
        if (verbose) {
            for (const auto& tok : tokens) {
                cout << tokenTypeToString(tok.type) << " ";
                if (tok.type == ID || tok.type == NUM) {
                    cout << "(" << tok.lexeme << ")";
                }
                cout << endl;
            }
            cout << endl;
        }
        
        // Fase 2: Análise Sintática
        if (verbose) cout << "=== ANÁLISE SINTÁTICA ===" << endl;
        Parser parser(tokens);
        ASTNode* ast = parser.parse();
        
        if (!ast) {
            cout << "REJEITADO" << endl;
            cout << parser.getError() << endl;
            return false;
        }
        
        if (verbose) cout << "Árvore sintática construída com sucesso." << endl << endl;
        
        // Fase 3: Análise Semântica
        if (verbose) cout << "=== ANÁLISE SEMÂNTICA ===" << endl;
        SemanticAnalyzer semantic;
        bool semanticOk = semantic.analyze(ast);
        
        if (!semanticOk) {
            cout << "REJEITADO" << endl;
            cout << semantic.getError() << endl;
            delete ast;
            return false;
        }
        
        if (verbose) cout << "Análise semântica concluída com sucesso." << endl << endl;
        
        // Sucesso!
        cout << "ACEITO" << endl;
        
        delete ast;
        return true;
    }
    
    bool compileFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cerr << "Erro: não foi possível abrir o arquivo '" << filename << "'" << endl;
            return false;
        }
        
        stringstream buffer;
        buffer << file.rdbuf();
        sourceCode = buffer.str();
        file.close();
        
        return compile(sourceCode);
    }
};

// ==================== MAIN ====================

bool hasNetoExtension(const string& filename) {
    if (filename.length() < 5) return false;
    return filename.substr(filename.length() - 5) == ".neto";
}

int main(int argc, char* argv[]) {
    bool verbose = false;
    string filename;
    
    // Processar argumentos
    for (int i = 1; i < argc; i++) {
        string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else {
            filename = arg;
        }
    }
    
    // Verificar se arquivo foi fornecido
    if (filename.empty()) {
        cerr << "Uso: " << argv[0] << " [-v] arquivo.neto" << endl;
        return 1;
    }
    
    // Verificar extensão .neto
    if (!hasNetoExtension(filename)) {
        cerr << "Erro: o arquivo deve ter extensão .neto" << endl;
        cerr << "Exemplo: " << argv[0] << " programa.neto" << endl;
        return 1;
    }
    
    Compiler compiler(verbose);
    compiler.compileFile(filename);
    
    return 0;
}

