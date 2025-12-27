CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99 -pedantic -O2 -I./include
CFLAGS_DEBUG = -Wall -Wextra -g -DDEBUG -fsanitize=address -fsanitize=undefined -I./include
LDFLAGS = -lm

# 源文件和目标文件
SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin
TEST_DIR = test

# 源文件列表
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/similarity

# 测试文件
TEST_SRCS = $(wildcard $(TEST_DIR)/test_*.c)
TEST_TARGETS = $(patsubst $(TEST_DIR)/test_%.c,$(BIN_DIR)/test_%,$(TEST_SRCS))

# 默认目标
.PHONY: all clean test run debug install help

all: $(TARGET)

# 主程序
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	@echo "Linking $@..."
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# 编译对象文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -c $< -o $@

# 测试程序
test: $(TEST_TARGETS)
	@echo "Running tests..."
	@for test in $(TEST_TARGETS); do \
		echo "Running $$test..."; \
		$$test || exit 1; \
	done
	@echo "All tests passed!"

$(BIN_DIR)/test_%: $(TEST_DIR)/test_%.c $(filter-out $(OBJ_DIR)/main.o,$(OBJS))
	@mkdir -p $(BIN_DIR)
	@echo "Building test: $@..."
	@$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# 清理
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf build/
	@echo "Clean complete!"

# 运行
run: $(TARGET)
	@echo "Running similarity analysis system..."
	@./$(TARGET)

# 调试版本
debug: CFLAGS = $(CFLAGS_DEBUG)
debug: clean all
	@echo "Debug build complete with sanitizers enabled"

# 安装（可选）
install: $(TARGET)
	@echo "Installing to /usr/local/bin..."
	@sudo cp $(TARGET) /usr/local/bin/
	@echo "Installation complete!"

# 帮助信息
help:
	@echo "Available targets:"
	@echo "  all      - Build the main program (default)"
	@echo "  test     - Build and run all tests"
	@echo "  clean    - Remove all build artifacts"
	@echo "  run      - Build and run the program"
	@echo "  debug    - Build with debug symbols and sanitizers"
	@echo "  install  - Install the program to /usr/local/bin"
	@echo "  help     - Show this help message"

# 依赖关系
-include $(OBJS:.o=.d)

$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	@$(CC) -MM -MT '$(OBJ_DIR)/$*.o' $(CFLAGS) $< > $@

# 性能分析版本
profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: clean all

# 安装
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# 生成文档
docs:
	doxygen Doxyfile

.PHONY: all clean run debug profile install docs test