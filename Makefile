CXX      := clang++
CXXFLAGS := -std=c++23 -Wall -Wextra -pedantic # newest version
TARGET   := program
BUILD_NAME := Abstract Syntax Tree

SRCS := main.cpp
OBJS := $(SRCS:.cpp=.o)

all: $(TARGET) info

info:
	@echo "======================================"
	@echo "$(BUILD_NAME) build complete."
	@echo "======================================"

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean info
