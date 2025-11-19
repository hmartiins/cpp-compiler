#include "compiler.h"
#include <iostream>
#include <string>

bool hasNetoExtension(const std::string& filename) {
    if (filename.length() < 5) return false;
    return filename.substr(filename.length() - 5) == ".neto";
}

int main(int argc, char* argv[]) {
    bool verbose = false;
    std::string filename;
    
    // Processar argumentos
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else {
            filename = arg;
        }
    }
    
    // Verificar se arquivo foi fornecido
    if (filename.empty()) {
        std::cerr << "Uso: " << argv[0] << " [-v] arquivo.neto" << std::endl;
        return 1;
    }
    
    // Verificar extensão .neto
    if (!hasNetoExtension(filename)) {
        std::cerr << "Erro: o arquivo deve ter extensão .neto" << std::endl;
        std::cerr << "Exemplo: " << argv[0] << " programa.neto" << std::endl;
        return 1;
    }
    
    Compiler compiler(verbose);
    compiler.compileFile(filename);
    
    return 0;
}

