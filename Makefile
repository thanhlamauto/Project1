# Makefile for Augmented Data Structures Project

CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
TARGET = main
SOURCES = main.cpp
HEADERS = ost.h pom.h josephus.h

all: $(TARGET)

$(TARGET): $(SOURCES) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
	rm -rf results/

test: $(TARGET)
	./$(TARGET)

visualize: run
	python3 visualize.py

.PHONY: all run clean test visualize

