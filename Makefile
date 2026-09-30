CXX := g++
CXXFLAGS := -std=c++17 -g -Wall -Wextra -fsanitize=address -MMD -MP -Iinclude
LDFLAGS := -fsanitize=address

BUILD_DIR := build
TARGET := $(BUILD_DIR)/airline_system

SRCS := $(shell find src -name '*.cpp')
OBJS := $(patsubst src/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS := $(OBJS:.o=.d)

.PHONY: all build run rebuild clean

all: build

build: $(TARGET)

$(TARGET): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJS) $(LDFLAGS)

$(BUILD_DIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Automatically include generated header dependency rules
-include $(DEPS)

run: build
	./$(TARGET)

# Clean, build, then run. sub-makes keep the order safe even under `make -j`
rebuild:
	$(MAKE) clean
	$(MAKE) build
	$(MAKE) run

clean:
	rm -rf $(BUILD_DIR)