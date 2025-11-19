# Makefile para o Mini Compilador

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = compiler

SOURCES = token.cpp lexer.cpp ast.cpp parser.cpp semantic.cpp compiler.cpp main.cpp utils.cpp
OBJECTS = $(SOURCES:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(OBJECTS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJECTS)

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

