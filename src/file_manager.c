#include "file_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define COLLECTION_INITIAL_CAPACITY 10

// 创建文档集合
DocumentCollection* collection_create(size_t capacity) {
    DocumentCollection *col = (DocumentCollection*)malloc(sizeof(DocumentCollection));
    if (!col) return NULL;
    
    col->capacity = capacity > 0 ? capacity : COLLECTION_INITIAL_CAPACITY;
    col->count = 0;
    col->documents = (Document**)malloc(col->capacity * sizeof(Document*));
    
    if (!col->documents) {
        free(col);
        return NULL;
    }
    
    return col;
}

// 向集合添加文档
bool collection_add_document(DocumentCollection *col, Document *doc) {
    if (!col || !doc) return false;
    
    if (col->count >= col->capacity) {
        col->capacity *= 2;
        Document **new_docs = realloc(col->documents, col->capacity * sizeof(Document*));
        if (!new_docs) return false;
        col->documents = new_docs;
    }
    
    col->documents[col->count++] = doc;
    return true;
}

// 销毁文档集合
void collection_destroy(DocumentCollection *col) {
    if (!col) return;
    
    for (size_t i = 0; i < col->count; i++) {
        document_destroy(col->documents[i]);
    }
    
    free(col->documents);
    free(col);
}

// 从目录加载文档
DocumentCollection* load_documents_from_dir(const char *dir_path, StopWords *stop_words) {
    DIR *dir = opendir(dir_path);
    if (!dir) {
        fprintf(stderr, "错误: 无法打开目录 %s\n", dir_path);
        return NULL;
    }
    
    DocumentCollection *col = collection_create(COLLECTION_INITIAL_CAPACITY);
    if (!col) {
        closedir(dir);
        return NULL;
    }
    
    struct dirent *entry;
    char filepath[512];
    
    while ((entry = readdir(dir)) != NULL) {
        // 检查文件扩展名
        char *dot = strrchr(entry->d_name, '.');
        if (!dot || strcmp(dot, ".txt") != 0) {
            continue;
        }
        
        // 构建完整路径
        snprintf(filepath, sizeof(filepath), "%s/%s", dir_path, entry->d_name);
        
        // 检查是否是文件
        struct stat path_stat;
        if (stat(filepath, &path_stat) != 0 || !S_ISREG(path_stat.st_mode)) {
            continue;
        }
        
        // 创建并处理文档
        Document *doc = document_create(entry->d_name);
        if (!doc) {
            continue;
        }
        
        if (document_load_from_file(doc, filepath) &&
            document_process(doc, stop_words)) {
            collection_add_document(col, doc);
            printf("已加载文档: %s\n", entry->d_name);
        } else {
            document_destroy(doc);
        }
    }
    
    closedir(dir);
    return col;
}

// 创建相似度矩阵
SimilarityMatrix* similarity_matrix_create(DocumentCollection *col) {
    if (!col || col->count == 0) return NULL;
    
    SimilarityMatrix *matrix = (SimilarityMatrix*)malloc(sizeof(SimilarityMatrix));
    if (!matrix) return NULL;
    
    matrix->size = col->count;
    matrix->filenames = (char**)malloc(matrix->size * sizeof(char*));
    matrix->matrix = (double**)malloc(matrix->size * sizeof(double*));
    
    if (!matrix->filenames || !matrix->matrix) {
        free(matrix->filenames);
        free(matrix->matrix);
        free(matrix);
        return NULL;
    }
    
    // 分配内存并初始化矩阵
    for (size_t i = 0; i < matrix->size; i++) {
        matrix->filenames[i] = strdup(col->documents[i]->filename);
        matrix->matrix[i] = (double*)calloc(matrix->size, sizeof(double));
        
        if (!matrix->filenames[i] || !matrix->matrix[i]) {
            // 清理已分配的内存
            for (size_t j = 0; j <= i; j++) {
                free(matrix->filenames[j]);
                free(matrix->matrix[j]);
            }
            free(matrix->filenames);
            free(matrix->matrix);
            free(matrix);
            return NULL;
        }
    }
    
    // 计算相似度
    for (size_t i = 0; i < matrix->size; i++) {
        matrix->matrix[i][i] = 1.0; // 对角线为1
        
        for (size_t j = i + 1; j < matrix->size; j++) {
            double similarity = document_cosine_similarity(
                col->documents[i], 
                col->documents[j]
            );
            
            matrix->matrix[i][j] = similarity;
            matrix->matrix[j][i] = similarity;
        }
    }
    
    return matrix;
}

// 销毁相似度矩阵
void similarity_matrix_destroy(SimilarityMatrix *matrix) {
    if (!matrix) return;
    
    for (size_t i = 0; i < matrix->size; i++) {
        free(matrix->filenames[i]);
        free(matrix->matrix[i]);
    }
    
    free(matrix->filenames);
    free(matrix->matrix);
    free(matrix);
}

// 保存相似度矩阵到CSV文件
bool similarity_matrix_save_csv(SimilarityMatrix *matrix, const char *filename) {
    if (!matrix || !filename) return false;
    
    FILE *file = fopen(filename, "w");
    if (!file) {
        fprintf(stderr, "错误: 无法创建文件 %s\n", filename);
        return false;
    }
    
    // 写入标题行
    fprintf(file, "Filename");
    for (size_t i = 0; i < matrix->size; i++) {
        fprintf(file, ",%s", matrix->filenames[i]);
    }
    fprintf(file, "\n");
    
    // 写入数据行
    for (size_t i = 0; i < matrix->size; i++) {
        fprintf(file, "%s", matrix->filenames[i]);
        
        for (size_t j = 0; j < matrix->size; j++) {
            fprintf(file, ",%.4f", matrix->matrix[i][j]);
        }
        
        fprintf(file, "\n");
    }
    
    fclose(file);
    printf("相似度矩阵已保存到 %s\n", filename);
    return true;
}

// 打印相似度矩阵
void similarity_matrix_print(SimilarityMatrix *matrix) {
    if (!matrix) {
        printf("矩阵为空\n");
        return;
    }
    
    printf("\n相似度矩阵 (%zux%zu):\n", matrix->size, matrix->size);
    printf("       ");
    
    // 打印列标题
    for (size_t i = 0; i < matrix->size && i < 8; i++) {
        printf("%8.8s ", matrix->filenames[i]);
    }
    if (matrix->size > 8) printf("...");
    printf("\n");
    
    // 打印数据
    for (size_t i = 0; i < matrix->size && i < 8; i++) {
        printf("%-8.8s ", matrix->filenames[i]);
        
        for (size_t j = 0; j < matrix->size && j < 8; j++) {
            printf("%8.4f ", matrix->matrix[i][j]);
        }
        
        if (matrix->size > 8) printf("...");
        printf("\n");
    }
    
    if (matrix->size > 8) {
        printf("... (显示前8行8列，完整矩阵已保存到文件)\n");
    }
}

// 比较函数用于排序
static int compare_similarity(const void *a, const void *b) {
    const SimilarityPair *pair1 = (const SimilarityPair*)a;
    const SimilarityPair *pair2 = (const SimilarityPair*)b;
    
    if (pair1->similarity > pair2->similarity) return -1;
    if (pair1->similarity < pair2->similarity) return 1;
    return 0;
}

// 查找前N个最相似对
SimilarityPair* find_top_similarities(SimilarityMatrix *matrix, size_t top_n, size_t *result_count) {
    if (!matrix || matrix->size < 2) return NULL;
    
    // 计算总对数
    size_t total_pairs = matrix->size * (matrix->size - 1) / 2;
    
    // 分配内存存储所有对
    SimilarityPair *all_pairs = (SimilarityPair*)malloc(total_pairs * sizeof(SimilarityPair));
    if (!all_pairs) return NULL;
    
    // 收集所有相似度对
    size_t pair_count = 0;
    for (size_t i = 0; i < matrix->size; i++) {
        for (size_t j = i + 1; j < matrix->size; j++) {
            strncpy(all_pairs[pair_count].doc1, matrix->filenames[i], 255);
            strncpy(all_pairs[pair_count].doc2, matrix->filenames[j], 255);
            all_pairs[pair_count].similarity = matrix->matrix[i][j];
            pair_count++;
        }
    }
    
    // 排序
    qsort(all_pairs, pair_count, sizeof(SimilarityPair), compare_similarity);
    
    // 返回前N个
    *result_count = pair_count < top_n ? pair_count : top_n;
    
    SimilarityPair *top_pairs = (SimilarityPair*)malloc(*result_count * sizeof(SimilarityPair));
    if (!top_pairs) {
        free(all_pairs);
        return NULL;
    }
    
    memcpy(top_pairs, all_pairs, *result_count * sizeof(SimilarityPair));
    free(all_pairs);
    
    return top_pairs;
}

// 排序相似度对
void sort_similarity_pairs(SimilarityPair *pairs, size_t count) {
    if (!pairs || count == 0) return;
    qsort(pairs, count, sizeof(SimilarityPair), compare_similarity);
}