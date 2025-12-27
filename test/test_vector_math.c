#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "vector_math.h"
#include "text_processor.h"

#define EPSILON 0.0001

void test_vector_creation() {
    printf("测试向量创建...\n");
    
    Vector *vec = vector_create(10);
    assert(vec != NULL);
    assert(vec->capacity == 10);
    assert(vec->size == 0);
    
    vector_destroy(vec);
    printf("向量创建测试通过！\n");
}

void test_vector_add() {
    printf("测试向量添加元素...\n");
    
    Vector *vec = vector_create(2);
    
    assert(vector_add(vec, 1.0));
    assert(vec->size == 1);
    assert(fabs(vec->data[0] - 1.0) < EPSILON);
    
    assert(vector_add(vec, 2.0));
    assert(vec->size == 2);
    
    // 测试自动扩容
    assert(vector_add(vec, 3.0));
    assert(vec->size == 3);
    assert(vec->capacity > 2);
    
    vector_destroy(vec);
    printf("向量添加测试通过！\n");
}

void test_vector_magnitude() {
    printf("测试向量模长计算...\n");
    
    Vector *vec = vector_create(10);
    
    vector_add(vec, 3.0);
    vector_add(vec, 4.0);
    
    double mag = vector_magnitude(vec);
    assert(fabs(mag - 5.0) < EPSILON); // sqrt(3^2 + 4^2) = 5
    
    vector_destroy(vec);
    printf("向量模长测试通过！\n");
}

void test_vector_normalize() {
    printf("测试向量归一化...\n");
    
    Vector *vec = vector_create(10);
    
    vector_add(vec, 3.0);
    vector_add(vec, 4.0);
    
    vector_normalize(vec);
    
    double mag = vector_magnitude(vec);
    assert(fabs(mag - 1.0) < EPSILON); // 归一化后模长为1
    
    vector_destroy(vec);
    printf("向量归一化测试通过！\n");
}

void test_vector_dot_product() {
    printf("测试向量点积...\n");
    
    Vector *vec1 = vector_create(10);
    Vector *vec2 = vector_create(10);
    
    vector_add(vec1, 1.0);
    vector_add(vec1, 2.0);
    vector_add(vec1, 3.0);
    
    vector_add(vec2, 4.0);
    vector_add(vec2, 5.0);
    vector_add(vec2, 6.0);
    
    double dot = vector_dot_product(vec1, vec2);
    assert(fabs(dot - 32.0) < EPSILON); // 1*4 + 2*5 + 3*6 = 32
    
    vector_destroy(vec1);
    vector_destroy(vec2);
    printf("向量点积测试通过！\n");
}

void test_cosine_similarity_vectors() {
    printf("测试余弦相似度（向量）...\n");
    
    // 测试完全相同的向量
    Vector *vec1 = vector_create(10);
    Vector *vec2 = vector_create(10);
    
    vector_add(vec1, 1.0);
    vector_add(vec1, 2.0);
    vector_add(vec1, 3.0);
    
    vector_add(vec2, 1.0);
    vector_add(vec2, 2.0);
    vector_add(vec2, 3.0);
    
    double sim = cosine_similarity(vec1, vec2);
    assert(fabs(sim - 1.0) < EPSILON); // 完全相同，相似度为1
    
    vector_destroy(vec1);
    vector_destroy(vec2);
    
    // 测试正交向量
    Vector *vec3 = vector_create(10);
    Vector *vec4 = vector_create(10);
    
    vector_add(vec3, 1.0);
    vector_add(vec3, 0.0);
    
    vector_add(vec4, 0.0);
    vector_add(vec4, 1.0);
    
    sim = cosine_similarity(vec3, vec4);
    assert(fabs(sim - 0.0) < EPSILON); // 正交，相似度为0
    
    vector_destroy(vec3);
    vector_destroy(vec4);
    
    printf("余弦相似度测试通过！\n");
}

void test_euclidean_distance() {
    printf("测试欧氏距离...\n");
    
    Vector *vec1 = vector_create(10);
    Vector *vec2 = vector_create(10);
    
    vector_add(vec1, 0.0);
    vector_add(vec1, 0.0);
    
    vector_add(vec2, 3.0);
    vector_add(vec2, 4.0);
    
    double dist = euclidean_distance(vec1, vec2);
    assert(fabs(dist - 5.0) < EPSILON); // sqrt((3-0)^2 + (4-0)^2) = 5
    
    vector_destroy(vec1);
    vector_destroy(vec2);
    printf("欧氏距离测试通过！\n");
}

void test_manhattan_distance() {
    printf("测试曼哈顿距离...\n");
    
    Vector *vec1 = vector_create(10);
    Vector *vec2 = vector_create(10);
    
    vector_add(vec1, 0.0);
    vector_add(vec1, 0.0);
    
    vector_add(vec2, 3.0);
    vector_add(vec2, 4.0);
    
    double dist = manhattan_distance(vec1, vec2);
    assert(fabs(dist - 7.0) < EPSILON); // |3-0| + |4-0| = 7
    
    vector_destroy(vec1);
    vector_destroy(vec2);
    printf("曼哈顿距离测试通过！\n");
}

void test_jaccard_similarity() {
    printf("测试Jaccard相似度...\n");
    
    // 创建两个测试文档
    Document *doc1 = document_create("doc1.txt");
    Document *doc2 = document_create("doc2.txt");
    
    // 文档1: {a:2, b:1, c:1}
    hash_table_insert(doc1->word_freq, "a", 2);
    hash_table_insert(doc1->word_freq, "b", 1);
    hash_table_insert(doc1->word_freq, "c", 1);
    
    // 文档2: {a:1, b:2, d:1}
    hash_table_insert(doc2->word_freq, "a", 1);
    hash_table_insert(doc2->word_freq, "b", 2);
    hash_table_insert(doc2->word_freq, "d", 1);
    
    double jac = jaccard_similarity(doc1, doc2);
    // 交集: {a, b} = 2
    // 并集: {a, b, c, d} = 4
    // Jaccard = 2/4 = 0.5
    assert(fabs(jac - 0.5) < EPSILON);
    
    document_destroy(doc1);
    document_destroy(doc2);
    printf("Jaccard相似度测试通过！\n");
}

void test_zero_vectors() {
    printf("测试零向量处理...\n");
    
    Vector *vec1 = vector_create(10);
    Vector *vec2 = vector_create(10);
    
    vector_add(vec1, 0.0);
    vector_add(vec1, 0.0);
    
    vector_add(vec2, 1.0);
    vector_add(vec2, 2.0);
    
    // 零向量的余弦相似度应该返回0或错误值
    double sim = cosine_similarity(vec1, vec2);
    assert(sim == 0.0);
    
    vector_destroy(vec1);
    vector_destroy(vec2);
    printf("零向量测试通过！\n");
}

void test_dimension_mismatch() {
    printf("测试维度不匹配...\n");
    
    Vector *vec1 = vector_create(10);
    Vector *vec2 = vector_create(10);
    
    vector_add(vec1, 1.0);
    vector_add(vec1, 2.0);
    
    vector_add(vec2, 3.0);
    vector_add(vec2, 4.0);
    vector_add(vec2, 5.0); // 不同维度
    
    // 维度不同应该返回错误值
    double dot = vector_dot_product(vec1, vec2);
    assert(dot == 0.0);
    
    vector_destroy(vec1);
    vector_destroy(vec2);
    printf("维度不匹配测试通过！\n");
}

int main() {
    printf("========================================\n");
    printf("向量数学测试套件\n");
    printf("========================================\n\n");
    
    test_vector_creation();
    test_vector_add();
    test_vector_magnitude();
    test_vector_normalize();
    test_vector_dot_product();
    test_cosine_similarity_vectors();
    test_euclidean_distance();
    test_manhattan_distance();
    test_jaccard_similarity();
    test_zero_vectors();
    test_dimension_mismatch();
    
    printf("\n========================================\n");
    printf("所有测试通过！✓\n");
    printf("========================================\n");
    
    return 0;
}
