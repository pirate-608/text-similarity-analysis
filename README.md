# 基于词频向量的文本相似度分析系统

[![Build Status](https://img.shields.io/badge/build-passing-brightgreen.svg)]()
[![C Standard](https://img.shields.io/badge/C-C99-blue.svg)]()
[![Python](https://img.shields.io/badge/Python-3.8%2B-blue.svg)]()
[![License](https://img.shields.io/badge/license-MIT-blue.svg)]()

## 项目概述

本项目是一个高性能的文本相似度分析系统，采用 **C 语言核心 + Python Web 界面** 的混合架构。底层使用 C 语言实现高效的词频向量化与相似度计算，上层通过 Python Flask 提供友好的 Web 交互界面，同时也支持传统的命令行（CLI）操作。

**最新版本**: v2.0 - 新增 Web 界面与 Python 绑定（2026-01-03）

## 功能特性

- **双模式运行**：
  - **Web 界面**：基于 Flask 的现代化 UI，支持拖拽上传、可视化矩阵展示、Top 相似对列表。
    - **公网访问**：支持使用 ngrok 将本地 Web 服务暴露给公网访问。
  - **CLI 工具**：高效的命令行工具，支持批处理与交互式菜单。
- **高性能核心**：
  - C 语言实现的文本预处理（分词、小写转换、停用词过滤）。
  - **多语言支持**：优化了分词算法，支持中文等非 ASCII 字符的统计。
  - 动态扩容哈希表存储词频。
  - 多种相似度计算方法（余弦、Jaccard、欧氏/曼哈顿距离）。
- **可视化**：
  - Web 端：颜色编码的相似度矩阵表格、Top 相似对排行、交互式数据展示。
  - CLI 端：ASCII 字符热力图。

## 依赖

- **C 核心**：
  - gcc/clang (支持 C99)
  - make
  - POSIX dirent.h (Windows 建议使用 MSYS2/MinGW 或 WSL)
- **Web 界面**：
  - Python 3.8+
  - Flask (`pip install flask`)

## 目录结构

- `src/`: C 语言核心源码
- `include/`: C 头文件
- `web/`: Python Web 应用与桥接代码
- `test/`: 单元测试
- `build/`: 构建产物
- `docs/`: 项目文档
  - `NGROK_GUIDE.md`: 公网访问配置指南

## 构建与安装

### 1. 编译 C 核心

```bash
# 编译 CLI 工具
make

# 编译 Web 界面所需的动态库 (DLL/SO)
make shared
```

### 2. 安装 Python 依赖 (仅 Web 模式)

```bash
pip install flask
```

## 运行指南

### 方式一：Web 界面模式（推荐）

启动 Web 服务后，在浏览器中进行可视化操作。

1.  确保已执行 `make shared`。
2.  启动服务：
    ```bash
    python web/app.py
    ```
3.  访问浏览器：`http://127.0.0.1:5000`

### 方式二：命令行交互模式

直接运行编译好的可执行文件，通过菜单进行操作。

```bash
./build/bin/similarity
```

### 方式三：命令行批处理模式

适合自动化脚本调用，直接生成 CSV 结果。

```bash
./build/bin/similarity -d ./samples/large -o result.csv
```

**常用参数**：
- `-d <目录>`：指定包含 .txt 文档的目录
- `-o <文件>`：指定输出 CSV 文件路径
- `-s <文件>`：指定自定义停用词表

## 文档资源

- [用户手册](docs/用户手册.md) - 详细的操作指南
- [设计文档](docs/设计文档.md) - 系统架构与实现细节
- [API 说明](docs/API说明.md) - C 接口与 Python 桥接说明
- [性能优化](docs/性能优化.md) - 性能调优指南

## 开发与测试

- **运行测试**：
  ```bash
  make test
  ```
- **调试构建**：
  ```bash
  make debug
  ```

## 贡献

欢迎提交 Issue 或 Pull Request。请查看 [CONTRIBUTING.md](CONTRIBUTING.md) 了解详情。

## 许可证

本项目采用 MIT 许可证 - 详见 LICENSE 文件。
