CC = gcc
CFLAGS = -Wall -Wextra -O2 -I./include
LDFLAGS = -lm

# 源文件和目标文件
SRC_DIR = src
OBJ_DIR = build/obj
BIN_DIR = build/bin

# 源文件列表
SRCS = $(wildcard $(SRC_DIR)/*.c)
OBJS = $(patsubst $(SRC_DIR)/%.c,$(OBJ_DIR)/%.o,$(SRCS))
TARGET = $(BIN_DIR)/similarity

# 测试文件
TEST_SRCS = $(wildcard test/*.c)
TEST_TARGETS = $(patsubst test/%.c,$(BIN_DIR)/test_%,$(TEST_SRCS))

# 默认目标
all: $(TARGET)

# 主程序
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $@ $(LDFLAGS)

# 编译对象文件
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# 测试程序
test: $(TEST_TARGETS)

$(BIN_DIR)/test_%: test/%.c $(filter-out $(OBJ_DIR)/main.o,$(OBJS))
	@mkdir -p $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@ $(LDFLAGS)

# 清理
clean:
	rm -rf build/

# 运行
run: $(TARGET)
	./$(TARGET)

# 调试版本
debug: CFLAGS += -g -DDEBUG
debug: clean all

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