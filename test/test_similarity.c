#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <math.h>
#include "text_processor.h"
#include "vector_math.h"

void test_document_processing() {
    printf("测试文档处理...\n");
    
    // 创建测试文档
    Document *doc = document_create("test.txt");
    assert(doc != NULL);
    
    // 模拟文档内容
    const char *test_content = "The quick brown fox jumps over the lazy dog. "
                               "The dog barks at the fox. Fox and dog!";
    
    doc->content = strdup(test_content);
    assert(doc->content != NULL);
    
    // 处理文档（不使用停用词）
    StopWords *stop_words = NULL; // 不使用停用词
    assert(document_process(doc, stop_words));
    
    // 验证词频
    assert(hash_table_get(doc->word_freq, "the") == 3);
    assert(hash_table_get(doc->word_freq, "fox") == 3);
    assert(hash_table_get(doc->word_freq, "dog") == 3);
    assert(hash_table_get(doc->word_freq, "quick") == 1);
    
    printf("文档总单词数: %zu\n", doc->word_count);
    printf("文档唯一单词数: %zu\n", doc->word_freq->unique_words);
    
    document_destroy(doc);
    printf("文档处理测试通过！\n");
}

void test_cosine_similarity() {
    printf("测试余弦相似度计算...\n");
    
    // 创建两个测试向量
    Vector *vec1 = vector_create(3);
    Vector *vec2 = vector_create(3);
    
    vector_add(vec1, 1.0);
    vector_add(vec1, 2.0);
    vector_add(vec1, 3.0);
    
    vector_add(vec2, 4.0);
    vector_add(vec2, 5.0);
    vector_add(vec2, 6.0);
    
    // 计算相似度
    double similarity = cosine_similarity(vec1, vec2);
    printf("向量相似度: %.4f\n", similarity);
    
    // 验证计算
    double expected = (1*4 + 2*5 + 3*6) / (sqrt(1+4+9) * sqrt(16+25+36));
    assert(fabs(similarity - expected) < 0.0001);
    
    vector_destroy(vec1);
    vector_destroy(vec2);
    
    printf("余弦相似度测试通过！\n");
}

void test_jaccard_similarity() {
    printf("测试Jaccard相似度...\n");
    
    HashTable *ht1 = hash_table_create(10);
    HashTable *ht2 = hash_table_create(10);
    
    hash_table_insert(ht1, "apple", 1);
    hash_table_insert(ht1, "banana", 1);
    hash_table_insert(ht1, "cherry", 1);
    
    hash_table_insert(ht2, "apple", 1);
    hash_table_insert(ht2, "banana", 1);
    hash_table_insert(ht2, "date", 1);
    
    double jaccard = jaccard_similarity(ht1, ht2);
    printf("Jaccard相似度: %.4f\n", jaccard);
    
    // 交集: {apple, banana} = 2
    // 并集: {apple, banana, cherry, date} = 4
    // 相似度: 2/4 = 0.5
    assert(fabs(jaccard - 0.5) < 0.0001);
    
    hash_table_destroy(ht1);
    hash_table_destroy(ht2);
    
    printf("Jaccard相似度测试通过！\n");
}

int main() {
    printf("开始相似度测试...\n\n");
    
    test_document_processing();
    printf("\n");
    
    test_cosine_similarity();
    printf("\n");
    
    test_jaccard_similarity();
    printf("\n");
    
    printf("所有相似度测试通过！\n");
    return 0;
}