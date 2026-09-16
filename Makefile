CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Iinclude
BUILD_DIR := build
TARGET := $(BUILD_DIR)/airline_system

SRCS := $(shell find src -name '*.cpp')
OBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: build run clean

build: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

run: build
	./$(TARGET)

clean:
	rm -rf $(BUILD_DIR)
