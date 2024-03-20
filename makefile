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
bucketFillThreshold ?= 75
bucketFillX ?= 800
bucketFillY ?= 170
resizeWidthBilinear ?= 500
resizeHeightBilinear ?= 745
resizeWidthBicubic ?= 500
resizeHeightBicubic ?= 745
resizeWidthNearestNeighbor ?= 500
resizeHeightNearestNeighbor ?= 745
inputImageSize ?= small
function ?= all

# Rule for running the executable with parameters
run: $(TARGET)
	./$(call FIXPATH,$(TARGET)) $(sigma) $(boxSize) $(motionLength) $(bucketFillThreshold) $(bucketFillX) $(bucketFillY) $(resizeWidthBilinear) $(resizeHeightBilinear) $(resizeWidthBicubic) $(resizeHeightBicubic) $(resizeWidthNearestNeighbor) $(resizeHeightNearestNeighbor) $(inputImageSize) $(function)

# Rule for cleaning up generated files
clean:
	$(RM) $(call FIXPATH,$(TARGET)) $(call FIXPATH,$(OBJECTS))

# Phony targets
.PHONY: run clean install-python-deps