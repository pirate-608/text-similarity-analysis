#ifndef VECTOR_MATH_H
#define VECTOR_MATH_H

#include "hashtable.h"
#include "text_processor.h"

// 向量结构
typedef struct Vector {
    double *data;
    size_t size;
    size_t capacity;
} Vector;

// 向量操作函数
Vector* vector_create(size_t capacity);
void vector_destroy(Vector *vec);
bool vector_add(Vector *vec, double value);
void vector_normalize(Vector *vec);
double vector_dot_product(Vector *vec1, Vector *vec2);
double vector_magnitude(Vector *vec);

// 相似度计算函数
double cosine_similarity(Vector *vec1, Vector *vec2);
double euclidean_distance(Vector *vec1, Vector *vec2);
double manhattan_distance(Vector *vec1, Vector *vec2);
double jaccard_similarity(HashTable *ht1, HashTable *ht2);

// 文档相似度函数
double document_cosine_similarity(Document *doc1, Document *doc2);
char** build_vocabulary(Document **docs, size_t doc_count, size_t *vocab_size);
void document_to_vector(Document *doc, Vector *vec, char **vocab, size_t vocab_size);

#endif