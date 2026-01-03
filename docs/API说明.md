# API 说明

## hashtable.h
- `HashTable* hash_table_create(size_t capacity)`：创建哈希表。
- `void hash_table_destroy(HashTable *table)`：销毁表并释放键。
- `bool hash_table_insert(HashTable *table, const char *key, int value)`：插入/累加词频。
- `int hash_table_get(HashTable *table, const char *key)`：获取词频，不存在返回 -1。
- `bool hash_table_remove(HashTable *table, const char *key)`：删除键。
- `void hash_table_resize(HashTable *table)`：按负载因子扩容。
- `double hash_table_load_factor(HashTable *table)`：返回负载因子。
- `void hash_table_print_stats(HashTable *table)`：打印统计。

## text_processor.h
- `Document* document_create(const char *filename)` / `void document_destroy(Document *doc)`。
- `bool document_load_from_file(Document *doc, const char *filename)`：读取文件内容。
- `bool document_process(Document *doc, StopWords *stop_words)`：分词、停用词过滤并填充哈希表。
- `void document_print_stats(Document *doc)`：打印单文档统计。
- 停用词：`StopWords* stop_words_create()`、`stop_words_load_from_file`、`stop_words_add`、`is_stop_word`、`stop_words_destroy`。
- 工具：`str_to_lower`、`is_word_char`、`get_next_word`。

## vector_math.h
- 向量：`vector_create`、`vector_destroy`、`vector_add`、`vector_normalize`、`vector_dot_product`、`vector_magnitude`。
- 相似度/距离：`cosine_similarity`、`euclidean_distance`、`manhattan_distance`、`jaccard_similarity`。
- 文档接口：`document_cosine_similarity`、`build_global_vector`（未实现）`document_to_vector`（未实现占位）。

## file_manager.h
- 集合：`collection_create`、`collection_add_document`、`collection_destroy`。
- `DocumentCollection* load_documents_from_dir(const char *dir_path, StopWords *stop_words)`：扫描 `.txt` 文件并处理。
- 矩阵：`similarity_matrix_create`、`similarity_matrix_destroy`、`similarity_matrix_save_csv`、`similarity_matrix_print`。
- 相似对：`find_top_similarities`（返回 Top-N 已排序副本）、`sort_similarity_pairs`。

## ui.h
- 菜单枚举 `MenuOption` 与交互函数：`print_menu`、`get_menu_choice`、`process_menu_choice`。
- 高阶操作：`compare_two_documents`、`show_statistics`、`show_top_similarity_pairs`、`filter_similarity_pairs`、`show_heatmap`。

## main.c
- CLI 参数：`-d` 目录启用批处理；`-o` 输出 CSV；`-s` 停用词文件；`-g` 预留 GUI；`-h` 帮助。
- `batch_mode`：加载目录 → 生成矩阵 → CSV → 输出 Top10。
- `interactive_mode`：循环菜单，依赖 `ui` 提供的操作。

## 错误与返回约定
- 资源创建失败返回 `NULL`/`false`/负值。
- 相似度函数在输入无效时返回 -1 或 0（详见实现）。
- 目录/文件读取失败会在 stderr 输出错误信息。

## 扩展示例
- 新增相似度函数：在 `vector_math.c` 添加实现，在 `file_manager` 中调用以填充矩阵；或在 `ui` 中增加菜单项。
- 新增输入过滤：在 `text_processor.c` 修改 `is_word_char` 与停用词逻辑。

## Web API (Python Bridge)
位于 `web/core_bridge.py`，通过 `ctypes` 封装 C 接口。

### `class SimilarityEngine`
- `__init__(self)`: 初始化引擎，加载动态库并创建停用词表。
- `process_directory(self, dir_path)`: 处理指定目录下的文档。
  - **参数**: `dir_path` (str) - 包含 `.txt` 文件的目录路径。
  - **返回**: `dict` - 包含 `filenames` (list of str) 和 `matrix` (list of list of float)。
  - **说明**: 自动调用 C 层的 `load_documents_from_dir` 和 `similarity_matrix_create`，并负责内存清理。

### REST API (Flask)
- `POST /analyze`
  - **Content-Type**: `multipart/form-data`
  - **参数**: `files[]` - 上传的一个或多个 `.txt` 文件。
  - **返回**: JSON 对象
    ```json
    {
      "filenames": ["doc1.txt", "doc2.txt"],
      "matrix": [[1.0, 0.5], [0.5, 1.0]]
    }
    ```
