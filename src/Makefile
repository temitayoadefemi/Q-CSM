# Compiler
CXX = g++

# Compiler Flags
# Add -g for debugging symbols if needed
CXXFLAGS = -std=c++17 -Wall -Wextra -pedantic

# Include Paths (-I flags)
# Added -I. for local headers
INCLUDES = -I/opt/homebrew/include -I.

# Linker Flags (-L flags)
LDFLAGS = -L/opt/homebrew/lib

# Libraries to Link (-l flags)
LDLIBS = -lcurl

# Executable Name
TARGET = q-csm_reflector

# Source Directories (for VPATH)
VPATH = q-csm:network_utils

SRCS = \
    main.cpp \
    q_state.cpp \
    q_emotion_engine.cpp \
    network_utils.cpp \

# Object Files (.o files) - Automatically generated from SRCS
OBJS = $(SRCS:.cpp=.o)

# Default target: build the executable
all: $(TARGET)

# Rule to link the executable
$(TARGET): $(OBJS)
	$(CXX) $(LDFLAGS) $^ -o $@ $(LDLIBS)
	@echo "Build complete: $(TARGET)"

# Pattern rule to compile .cpp files into .o object files
# Make will use VPATH to find the source files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

# Rule to clean up generated files
clean:
	@echo "Cleaning up..."
	rm -f $(OBJS) $(TARGET)

# Declare 'all' and 'clean' as phony targets (targets that don't represent actual files)
.PHONY: all clean