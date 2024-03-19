# Detect OS
OS := $(shell uname -s)

# Specify the compiler
CXX=g++

# Compiler flags
CXXFLAGS=-std=c++2a -pthread

# Target executable name
ifeq ($(OS),Windows_NT)
    TARGET=image-processor.exe
    RM=del /Q
    FIXPATH = $(subst /,\,$1)
    PIP=pip
else
    TARGET=image-processor
    RM=rm -f
    FIXPATH = $1
    PIP=pip
endif

# Source files
SOURCES=image-processor.cpp

# Object files
OBJECTS=$(SOURCES:.cpp=.o)

# ImGui directory
IMGUI_DIR=./libs/imgui

# Install Python dependencies
install-python-deps:
	$(PIP) install pillow

# Rule for building the executable
$(TARGET): install-python-deps $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(TARGET)

# Rule for running the executable with an optional function name argument
run: $(TARGET)
	./$(call FIXPATH,$(TARGET)) $(ARGS)

# Rule for cleaning up generated files
clean:
	$(RM) $(call FIXPATH,$(TARGET)) $(call FIXPATH,$(OBJECTS))

# Phony targets
.PHONY: run clean check-imgui install-python-deps
