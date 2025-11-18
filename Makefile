# Makefile para o Mini Compilador

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = compiler
SOURCE = compiler.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)

clean:
	rm -f $(TARGET)

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

