# 基于词频向量的文本相似度分析系统

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)]()
[![License](https://img.shields.io/badge/license-MIT-blue.svg)]()

## 项目概述

本项目用 C 语言实现词频向量化与相似度计算，通过余弦/Jaccard 等指标评估文本相似度，支持批处理与交互式 CLI。

**最新版本**: v1.1 - 改进了安全性、测试覆盖和文档（2025-12-27）

## 功能特性

- 文本预处理（分词、小写转换、停用词过滤）
- 动态扩容哈希表存储词频
- 多种相似度计算方法（余弦、Jaccard、欧氏/曼哈顿距离）
- 批量处理文档目录并生成相似度矩阵（CSV 导出）
- 交互式命令行界面与 ASCII 热力图

## 依赖

- gcc/clang，支持 C99；make
- POSIX dirent.h。Windows 建议使用 MSYS2/MinGW 或 WSL 以获得兼容的 dirent 实现。

## 目录结构

- src/: 主程序与核心逻辑
- include/: 头文件
- test/: 单元测试
- build/: 构建产物（首次构建后生成）

## 构建

```bash
make
```

生成的可执行文件位于 build/bin/similarity。

## 运行

- 交互模式（默认）：

  ```bash
  ./build/bin/similarity
  ```

- 批处理模式（自动生成 CSV，相似对输出 Top 10）：

  ```bash
  ./build/bin/similarity -d ./data -o similarity.csv -s stopwords.txt
  ```

常用参数：

- -d <目录>：包含 .txt 文档的目录（指定后进入批处理模式）
- -o <文件>：输出相似度矩阵 CSV 路径，默认 similarity_matrix.csv
- -s <文件>：额外停用词列表（逐行一个单词）
- -g：预留图形界面开关（当前未实现）

## 测试

```bash
make test
./build/bin/test_hashtable
./build/bin/test_similarity
```
- 单个文件大小限制为 100MB。

## 开发

### 代码质量

- 使用 `make debug` 构建调试版本（包含 AddressSanitizer）
- 运行 `make test` 执行所有单元测试
- 遵循 [贡献指南](CONTRIBUTING.md) 中的代码规范

### 性能优化

查看 [性能优化指南](docs/性能优化.md) 了解：
- 性能分析工具使用
- 优化技术和最佳实践
- 并行化和高级优化

### 项目文档

- [设计文档](docs/设计文档.md) - 架构和设计说明
- [API 说明](docs/API说明.md) - 完整的 API 参考
- [用户手册](docs/用户手册.md) - 详细使用说明
- [改进总结](IMPROVEMENTS.md) - 最新改进详情

## 贡献

欢迎贡献！请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解如何参与项目开发。

## 许可证

本项目采用 MIT 许可证 - 详见 LICENSE 文件。

## 更新日志

### v1.1 (2025-12-27)
- ✅ 改进了代码安全性和错误处理
- ✅ 增强了构建系统和测试覆盖
- ✅ 完善了文档和贡献指南
- ✅ 添加了性能优化指南
- ✅ 新增 20+ 个测试用例

### v1.0 (初始版本)
- 基本文本相似度分析功能
- 交互式和批处理模式
- 多种相似度算法支持

## 提示

- 文档文件需为 UTF-8 文本，扩展名 .txt。
- 批处理模式会忽略非 .txt 文件并自动跳过非普通文件。
- 生成的相似度矩阵在大规模文档时可能较大，可通过交互式菜单的热力图先查看前 20x20 子矩阵。
