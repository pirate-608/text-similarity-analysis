# 贡献指南

感谢您对文本相似度分析系统的关注！

## 如何贡献

### 报告问题

如果您发现 bug 或有功能建议：
1. 检查是否已有类似的 issue
2. 创建新 issue，包含：
   - 问题描述
   - 复现步骤
   - 预期行为
   - 实际行为
   - 环境信息（OS、编译器版本等）

### 提交代码

1. Fork 本仓库
2. 创建特性分支 (`git checkout -b feature/amazing-feature`)
3. 编写代码并遵循代码规范
4. 添加或更新测试
5. 确保所有测试通过 (`make test`)
6. 提交更改 (`git commit -m 'Add some amazing feature'`)
7. 推送到分支 (`git push origin feature/amazing-feature`)
8. 创建 Pull Request

## 代码规范

### C 代码风格

- 使用 4 空格缩进（不使用 tab）
- 使用 snake_case 命名函数和变量
- 使用 PascalCase 命名结构体类型
- 函数左花括号另起一行
- 添加适当的注释说明复杂逻辑
- 保持函数简短（建议 < 50 行）

### 示例

```c
// 好的风格
void process_document(Document *doc) {
    if (!doc) {
        fprintf(stderr, "错误: 空指针\n");
        return;
    }
    
    // 处理逻辑...
}

// 避免的风格
void processDoc(Document* doc){
    if(!doc)return;
    // ...
}
```

### 内存管理

- 每个 `malloc` 必须有对应的 `free`
- 检查所有内存分配是否成功
- 避免内存泄漏和重复释放
- 使用 valgrind 或 AddressSanitizer 检测内存问题

```bash
# 使用 AddressSanitizer
make debug
./build/bin/similarity
```

### 错误处理

- 检查所有可能失败的操作
- 使用 stderr 输出错误信息
- 返回适当的错误码或 NULL
- 在失败时清理已分配的资源

### 测试

- 为新功能添加单元测试
- 测试边界情况和错误条件
- 确保测试可重复运行
- 测试文件命名：`test_<module>.c`

```c
void test_new_feature() {
    printf("测试新功能...\n");
    
    // 设置
    MyStruct *obj = my_struct_create();
    assert(obj != NULL);
    
    // 测试
    bool result = my_function(obj);
    assert(result == true);
    
    // 清理
    my_struct_destroy(obj);
    
    printf("测试通过！\n");
}
```

## 开发环境设置

### Linux/macOS

```bash
# 安装依赖
sudo apt-get install gcc make  # Ubuntu/Debian
brew install gcc make          # macOS

# 克隆仓库
git clone <repository-url>
cd text-similarity-analysis

# 构建
make

# 运行测试
make test

# 调试构建
make debug
```

### Windows

推荐使用 MSYS2/MinGW 或 WSL：

```bash
# MSYS2
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make

# 然后按照 Linux 步骤操作
```

## 项目结构

```
├── src/           # 源代码
├── include/       # 头文件
├── test/          # 测试代码
├── docs/          # 文档
├── samples/       # 示例数据
└── build/         # 构建产物（自动生成）
```

## 添加新功能示例

### 添加新的相似度算法

1. 在 [vector_math.h](../include/vector_math.h) 添加函数声明：

```c
double my_similarity(Document *doc1, Document *doc2);
```

2. 在 [vector_math.c](../src/vector_math.c) 实现：

```c
double my_similarity(Document *doc1, Document *doc2) {
    if (!doc1 || !doc2) return -1.0;
    
    // 实现您的算法
    // ...
    
    return similarity_score;
}
```

3. 添加测试到 [test/test_similarity.c](../test/test_similarity.c)

4. 更新文档

## 提交信息规范

使用清晰的提交信息：

- `feat: 添加新功能`
- `fix: 修复 bug`
- `docs: 更新文档`
- `test: 添加或修改测试`
- `refactor: 重构代码`
- `perf: 性能优化`
- `style: 代码格式调整`

## 许可证

贡献的代码将遵循项目的开源许可证。

## 联系方式

如有问题，请通过 issue 或邮件联系维护者。

感谢您的贡献！ 🎉
