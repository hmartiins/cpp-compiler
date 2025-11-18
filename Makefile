# Makefile para o Mini Compilador

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
TARGET = compiler
SOURCE = compiler.cpp

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SOURCE)
