# Specify the compiler
CXX=g++

# Compiler flags
CXXFLAGS=-std=c++2a -pthread

# Target executable name
TARGET=image-processor

# Source files
SOURCES=image-processor.cpp

# Object files
OBJECTS=$(SOURCES:.cpp=.o)

# Rule for building the executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Rule for running the executable
run: $(TARGET)
	./$(TARGET)

# Rule for cleaning up generated files
clean:
	rm -f $(TARGET) $(OBJECTS)

# Phony targets
.PHONY: run clean