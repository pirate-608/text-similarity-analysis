#include "vector_math.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define VECTOR_INITIAL_CAPACITY 100

// 创建向量
Vector* vector_create(size_t capacity) {
    Vector *vec = (Vector*)malloc(sizeof(Vector));
    if (!vec) return NULL;
    
    vec->capacity = capacity > 0 ? capacity : VECTOR_INITIAL_CAPACITY;
    vec->size = 0;
    vec->data = (double*)calloc(vec->capacity, sizeof(double));
    
    if (!vec->data) {
        free(vec);
        return NULL;
    }
    
    return vec;
}

// 销毁向量
void vector_destroy(Vector *vec) {
    if (!vec) return;
    
    if (vec->data) {
        free(vec->data);
    }
    
    free(vec);
}

// 向向量添加元素
bool vector_add(Vector *vec, double value) {
    if (!vec) return false;
    
    if (vec->size >= vec->capacity) {
        vec->capacity *= 2;
        double *new_data = realloc(vec->data, vec->capacity * sizeof(double));
        if (!new_data) return false;
        vec->data = new_data;
    }
    
    vec->data[vec->size++] = value;
    return true;
}

// 向量归一化
void vector_normalize(Vector *vec) {
    if (!vec || vec->size == 0) return;
    
    double mag = vector_magnitude(vec);
    if (mag == 0) return;
    
    for (size_t i = 0; i < vec->size; i++) {
        vec->data[i] /= mag;
    }
}

// 计算点积
double vector_dot_product(Vector *vec1, Vector *vec2) {
    if (!vec1 || !vec2 || vec1->size != vec2->size) {
        return 0.0;
    }
    
    double result = 0.0;
    for (size_t i = 0; i < vec1->size; i++) {
        result += vec1->data[i] * vec2->data[i];
    }
    
    return result;
}

// 计算向量模长
double vector_magnitude(Vector *vec) {
    if (!vec || vec->size == 0) return 0.0;
    
    double sum = 0.0;
    for (size_t i = 0; i < vec->size; i++) {
        sum += vec->data[i] * vec->data[i];
    }
    
    return sqrt(sum);
}

// 计算余弦相似度
double cosine_similarity(Vector *vec1, Vector *vec2) {
    if (!vec1 || !vec2 || vec1->size != vec2->size) {
        return -1.0; // 错误值
    }
    
    double dot = vector_dot_product(vec1, vec2);
    double mag1 = vector_magnitude(vec1);
    double mag2 = vector_magnitude(vec2);
    
    if (mag1 == 0 || mag2 == 0) {
        return 0.0;
    }
    
    return dot / (mag1 * mag2);
}

// 计算欧氏距离
double euclidean_distance(Vector *vec1, Vector *vec2) {
    if (!vec1 || !vec2 || vec1->size != vec2->size) {
        return -1.0;
    }
    
    double sum = 0.0;
    for (size_t i = 0; i < vec1->size; i++) {
        double diff = vec1->data[i] - vec2->data[i];
        sum += diff * diff;
    }
    
    return sqrt(sum);
}

// 计算曼哈顿距离
double manhattan_distance(Vector *vec1, Vector *vec2) {
    if (!vec1 || !vec2 || vec1->size != vec2->size) {
        return -1.0;
    }
    
    double sum = 0.0;
    for (size_t i = 0; i < vec1->size; i++) {
        sum += fabs(vec1->data[i] - vec2->data[i]);
    }
    
    return sum;
}

// 计算Jaccard相似度
double jaccard_similarity(HashTable *ht1, HashTable *ht2) {
    if (!ht1 || !ht2) return 0.0;
    
    size_t intersection = 0;
    size_t union_size = 0;
    
    // 遍历哈希表1
    for (size_t i = 0; i < ht1->capacity; i++) {
        Entry *entry = ht1->buckets[i];
        while (entry) {
            if (hash_table_get(ht2, entry->key) != -1) {
                intersection++;
            }
            union_size++;
            entry = entry->next;
        }
    }
    
    // 添加哈希表2中独有的元素
    for (size_t i = 0; i < ht2->capacity; i++) {
        Entry *entry = ht2->buckets[i];
        while (entry) {
            if (hash_table_get(ht1, entry->key) == -1) {
                union_size++;
            }
            entry = entry->next;
        }
    }
    
    if (union_size == 0) return 0.0;
    return (double)intersection / union_size;
}

// 计算文档余弦相似度
double document_cosine_similarity(Document *doc1, Document *doc2) {
    if (!doc1 || !doc2 || !doc1->word_freq || !doc2->word_freq) {
        return -1.0;
    }
    
    // 获取两个哈希表的所有键
    HashTable *combined = hash_table_create(101);
    
    // 收集所有单词
    for (size_t i = 0; i < doc1->word_freq->capacity; i++) {
        Entry *entry = doc1->word_freq->buckets[i];
        while (entry) {
            hash_table_insert(combined, entry->key, 1);
            entry = entry->next;
        }
    }
    
    for (size_t i = 0; i < doc2->word_freq->capacity; i++) {
        Entry *entry = doc2->word_freq->buckets[i];
        while (entry) {
            hash_table_insert(combined, entry->key, 1);
            entry = entry->next;
        }
    }
    
    // 创建向量
    Vector *vec1 = vector_create(combined->unique_words);
    Vector *vec2 = vector_create(combined->unique_words);
    
    if (!vec1 || !vec2) {
        hash_table_destroy(combined);
        if (vec1) vector_destroy(vec1);
        if (vec2) vector_destroy(vec2);
        return -1.0;
    }
    
    // 填充向量
    for (size_t i = 0; i < combined->capacity; i++) {
        Entry *entry = combined->buckets[i];
        while (entry) {
            int freq1 = hash_table_get(doc1->word_freq, entry->key);
            int freq2 = hash_table_get(doc2->word_freq, entry->key);
            
            vector_add(vec1, freq1 > 0 ? (double)freq1 : 0.0);
            vector_add(vec2, freq2 > 0 ? (double)freq2 : 0.0);
            
            entry = entry->next;
        }
    }
    
    // 计算相似度
    double similarity = cosine_similarity(vec1, vec2);
    
    // 清理
    hash_table_destroy(combined);
    vector_destroy(vec1);
    vector_destroy(vec2);
    
    return similarity;
}