# 快速参考

## 常用命令

### 构建和运行

```bash
# 构建 CLI 工具
make

# 构建 Web 界面依赖库
make shared

# 运行 CLI 程序（交互模式）
make run
# 或
./build/bin/similarity

# 运行 Web 界面
python web/app.py
# 访问 http://127.0.0.1:5000

# 批处理模式 (CLI)
./build/bin/similarity -d ./samples/small -o results.csv
```

### 测试

```bash
# 运行所有测试
make test

# 单独运行某个测试
./build/bin/test_hashtable
./build/bin/test_text_processor
./build/bin/test_vector_math
```

### 调试

```bash
# 调试构建（含 AddressSanitizer）
make debug
./build/bin/similarity

# 内存检查（需要 valgrind）
valgrind --leak-check=full ./build/bin/similarity
```

### 清理

```bash
# 清理所有构建产物
make clean
```

---

## 命令行参数

| 参数 | 说明 | 示例 |
|------|------|------|
| `-d <目录>` | 指定文档目录（启用批处理） | `-d ./data` |
| `-o <文件>` | 输出 CSV 文件名 | `-o output.csv` |
| `-s <文件>` | 停用词文件 | `-s stopwords.txt` |
| `-h` | 帮助信息 | `-h` |

**注意：** 图形界面请使用 Web 模式 (`python web/app.py`)。

**示例：**
```bash
# 基本批处理
./build/bin/similarity -d ./samples/mini

# 自定义输出和停用词
./build/bin/similarity -d ./data -o analysis.csv -s custom_stopwords.txt
```

---

## 交互模式菜单

| 选项 | 功能 |
|------|------|
| 1 | 加载文档目录 |
| 2 | 生成相似度矩阵 |
| 3 | 保存矩阵到 CSV |
| 4 | 显示 Top-N 相似文档对 |
| 5 | 显示热力图 |
| 6 | 显示统计信息 |
| 0 | 退出 |

---
Web 界面操作

1.  **启动服务**：
    ```bash
    make shared
    pip install flask
    python web/app.py
    ```
2.  **使用功能**：
    - 打开浏览器访问 `http://127.0.0.1:5000`
    - 点击 "选择文件" 上传多个 `.txt` 文档
    - 点击 "开始分析"
    - 查看生成的相似度矩阵和热力图

---

## 
## 核心 API

### 文档处理

```c
// 创建文档
Document *doc = document_create("file.txt");

// 加载和处理
document_load_from_file(doc, "path/to/file.txt");
document_process(doc, stop_words);

// 清理
document_destroy(doc);
```

### 停用词

```c
// 创建停用词表（含默认词）
StopWords *sw = stop_words_create();

// 从文件加载额外停用词
stop_words_load_from_file(sw, "stopwords.txt");

// 清理
stop_words_destroy(sw);
```

### 相似度计算

```c
// 余弦相似度
double cos_sim = cosine_similarity_documents(doc1, doc2);

// Jaccard 相似度
double jac_sim = jaccard_similarity(doc1, doc2);
```

### 批量处理

```c
// 加载目录
DocumentCollection *col = load_documents_from_dir("./data", stop_words);

// 生成相似度矩阵
SimilarityMatrix *matrix = similarity_matrix_create(col);

// 保存到 CSV
similarity_matrix_save_csv(matrix, "output.csv");

// 查找 Top-N
size_t count;
SimilarityPair *pairs = find_top_similarities(matrix, 10, &count);

// 清理
free(pairs);
similarity_matrix_destroy(matrix);
collection_destroy(col);
```

---

## 文件格式

### 输入文档
- 格式：UTF-8 纯文本
- 扩展名：`.txt`
- 大小限制：100MB

### 停用词文件
```
the
a
an
and
```
（每行一个单词）

### 输出 CSV
```csv
,doc1.txt,doc2.txt,doc3.txt
doc1.txt,1.0000,0.8532,0.6421
doc2.txt,0.8532,1.0000,0.7234
doc3.txt,0.6421,0.7234,1.0000
```

---

## 性能提示

### 小规模数据集（< 100 文档）
- 使用默认设置即可
- 交互模式体验更好

### 中等规模（100-1000 文档）
- 使用批处理模式
- 考虑使用停用词减少词汇量
- 预期生成时间：几秒到几十秒

### 大规模数据集（> 1000 文档）
- 批处理模式必须
- 考虑分批处理
- 相似度矩阵会很大（n² 空间复杂度）
- 参考 [性能优化指南](docs/性能优化.md)

---

## 故障排除

### 编译错误

**问题：** `dirent.h not found` (Windows)
```bash
# 解决方案：使用 MSYS2/MinGW 或 WSL
# MSYS2 安装：
pacman -S mingw-w64-x86_64-gcc
```

**问题：** 链接错误 `-lm`
```bash
# 确保安装了数学库（通常默认有）
sudo apt-get install build-essential
```

### 运行时错误

**问题：** "无法打开文件"
- 检查文件路径是否正确
- 确认文件扩展名为 `.txt`
- 验证文件编码为 UTF-8

**问题：** "文件太大"
- 单个文件限制 100MB
- 考虑分割大文件

**问题：** 内存不足
- 减少同时处理的文档数量
- 使用更强大的机器
- 考虑流式处理（需要代码修改）

---

## 快速诊断

```bash
# 检查构建
make clean && make
echo $?  # 应该返回 0

# 运行测试
make test
# 所有测试应该通过

# 内存检查
make debug
./build/bin/similarity -d ./samples/mini
# 不应有内存泄漏

# 性能测试
time ./build/bin/similarity -d ./samples/small
```

---

## 相关文档

- 📖 [完整用户手册](docs/用户手册.md)
- 🔧 [API 参考](docs/API说明.md)
- 🏗️ [设计文档](docs/设计文档.md)
- ⚡ [性能优化](docs/性能优化.md)
- 🤝 [贡献指南](CONTRIBUTING.md)
- 📝 [改进总结](IMPROVEMENTS.md)

---

## 联系和支持

- 📋 问题反馈：提交 GitHub Issue
- 💬 讨论：GitHub Discussions
- 📧 邮件：[待填写]

---

**提示：** 使用 `make help` 查看所有可用的构建目标。
