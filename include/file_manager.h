#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include "text_processor.h"
#include "vector_math.h"
#include <stdbool.h>

// 文档集合
typedef struct DocumentCollection {
    Document **documents;
    size_t count;
    size_t capacity;
} DocumentCollection;

// 相似度矩阵
typedef struct SimilarityMatrix {
    double **matrix;
    char **filenames;
    size_t size;
} SimilarityMatrix;

// 相似度对
typedef struct SimilarityPair {
    char doc1[256];
    char doc2[256];
    double similarity;
} SimilarityPair;

// 文档集合函数
DocumentCollection* collection_create(size_t capacity);
bool collection_add_document(DocumentCollection *col, Document *doc);
void collection_destroy(DocumentCollection *col);
DocumentCollection* load_documents_from_dir(const char *dir_path, StopWords *stop_words);

// 相似度矩阵函数
SimilarityMatrix* similarity_matrix_create(DocumentCollection *col);
void similarity_matrix_destroy(SimilarityMatrix *matrix);
bool similarity_matrix_save_csv(SimilarityMatrix *matrix, const char *filename);
void similarity_matrix_print(SimilarityMatrix *matrix);

// 相似度对函数
SimilarityPair* find_top_similarities(SimilarityMatrix *matrix, size_t top_n, size_t *result_count);
void sort_similarity_pairs(SimilarityPair *pairs, size_t count);

#endif