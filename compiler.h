#ifndef COMPILER_H
#define COMPILER_H

#include <string>

class Compiler {
private:
    std::string sourceCode;
    bool verbose;
    
public:
    Compiler(bool v = false);
    bool compile(const std::string& source);
    bool compileFile(const std::string& filename);
};

#endif // COMPILER_H

