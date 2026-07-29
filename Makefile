# Build the JSON query tool.
#
#   make          build bin/jsonquery
#   make run      build, then run it from the project root
#   make clean    remove build artifacts

CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -MMD -MP
LDFLAGS  :=

SRC_DIR   := src
BUILD_DIR := build
BIN       := bin/jsonquery

SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(SRCS:$(SRC_DIR)/%.cpp=$(BUILD_DIR)/%.o)
DEPS := $(OBJS:.o=.d)

all: $(BIN)

$(BIN): $(OBJS)
	@mkdir -p $(dir $@)
	$(CXX) $(LDFLAGS) $(OBJS) -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# The program opens src/jsonFiles/<name>, so it must run from the project root.
run: $(BIN)
	./$(BIN)

debug: CXXFLAGS += -g -O0 -fsanitize=address,undefined
debug: LDFLAGS  += -fsanitize=address,undefined
debug: clean all

clean:
	rm -rf $(BUILD_DIR) bin

-include $(DEPS)

.PHONY: all run debug clean
