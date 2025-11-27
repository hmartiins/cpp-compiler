# Makefile para o Mini Compilador com Bison/Flex

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
FLEX = flex
BISON = bison
TARGET = compiler

# Arquivos gerados pelo Bison/Flex
BISON_GEN = parser.tab.cc parser.tab.hh location.hh position.hh stack.hh
FLEX_GEN = lex.yy.cc

# Fontes originais
ORIGINAL_SOURCES = token.cpp ast.cpp semantic.cpp compiler.cpp main.cpp utils.cpp parser_interface.cpp
ORIGINAL_OBJECTS = $(ORIGINAL_SOURCES:.cpp=.o)

# Objetos gerados
GENERATED_OBJECTS = parser.tab.o lex.yy.o

# Todos os objetos
OBJECTS = $(ORIGINAL_OBJECTS) $(GENERATED_OBJECTS)

all: $(TARGET)

# Regra para gerar arquivos do Bison (deve ser gerado primeiro)
parser.tab.cc parser.tab.hh: parser.y
	$(BISON) -d -o parser.tab.cc parser.y

# Regra para gerar arquivos do Flex (depende do parser.tab.hh)
lex.yy.cc: lexer.l parser.tab.hh
	$(FLEX) -o lex.yy.cc lexer.l

# Garantir que os arquivos gerados existam antes de compilar
parser_interface.o: parser_interface.cpp parser.tab.hh
	$(CXX) $(CXXFLAGS) -c parser_interface.cpp -o parser_interface.o

# Compilar objetos gerados
parser.tab.o: parser.tab.cc parser.tab.hh
	$(CXX) $(CXXFLAGS) -c parser.tab.cc -o parser.tab.o

lex.yy.o: lex.yy.cc parser.tab.hh
	$(CXX) $(CXXFLAGS) -c lex.yy.cc -o lex.yy.o

# Compilar objetos originais
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Linkar tudo
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

clean:
	rm -f $(TARGET) $(OBJECTS) $(BISON_GEN) $(FLEX_GEN)

test: $(TARGET)
	@echo "=== Testando código válido ==="
	@./$(TARGET) examples/valid1.neto
	@echo ""
	@./$(TARGET) examples/valid2.neto
	@echo ""
	@./$(TARGET) examples/valid3.neto
	@echo ""
	@echo "=== Testando código inválido ==="
	@./$(TARGET) examples/error_lexical.neto
	@echo ""
	@./$(TARGET) examples/error_syntax.neto
	@echo ""
	@./$(TARGET) examples/error_semantic.neto

.PHONY: all clean test
