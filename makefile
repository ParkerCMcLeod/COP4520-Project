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

# Default parameter values
sigma ?= 3.0
boxSize ?= 9
motionLength ?= 15
bucketFillThreshold ?= 10
bucketFillX ?= 504
bucketFillY ?= 341
resizeWidth ?= 800
resizeHeight ?= 600
inputImageSize ?= small
function ?= all

# Rule for running the executable with parameters
run: $(TARGET)
	./$(call FIXPATH,$(TARGET)) $(sigma) $(boxSize) $(motionLength) $(bucketFillThreshold) $(bucketFillX) $(bucketFillY) $(resizeWidth) $(resizeHeight) $(inputImageSize) $(function)

# Rule for cleaning up generated files
clean:
	$(RM) $(call FIXPATH,$(TARGET)) $(call FIXPATH,$(OBJECTS))

# Phony targets
.PHONY: run clean check-imgui install-python-deps