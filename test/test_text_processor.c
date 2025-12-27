#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "text_processor.h"

void test_stop_words_basic() {
    printf("测试停用词基本功能...\n");
    
    StopWords *sw = stop_words_create();
    assert(sw != NULL);
    assert(sw->size > 0); // 应该包含默认停用词
    
    // 测试默认停用词
    assert(is_stop_word(sw, "the") == true);
    assert(is_stop_word(sw, "and") == true);
    assert(is_stop_word(sw, "hello") == false);
    
    // 添加自定义停用词
    assert(stop_words_add(sw, "custom"));
    assert(is_stop_word(sw, "custom") == true);
    
    stop_words_destroy(sw);
    printf("停用词基本测试通过！\n");
}

void test_document_creation() {
    printf("测试文档创建...\n");
    
    Document *doc = document_create("test.txt");
    assert(doc != NULL);
    assert(doc->word_freq != NULL);
    assert(strcmp(doc->filename, "test.txt") == 0);
    assert(doc->word_count == 0);
    
    document_destroy(doc);
    printf("文档创建测试通过！\n");
}

void test_word_tokenization() {
    printf("测试分词功能...\n");
    
    char text[] = "Hello World! This is a test.";
    char *text_ptr = text;
    
    char *word1 = get_next_word(&text_ptr);
    assert(word1 != NULL);
    assert(strcmp(word1, "Hello") == 0);
    free(word1);
    
    char *word2 = get_next_word(&text_ptr);
    assert(word2 != NULL);
    assert(strcmp(word2, "World") == 0);
    free(word2);
    
    printf("分词测试通过！\n");
}

void test_str_to_lower() {
    printf("测试小写转换...\n");
    
    char str1[] = "HELLO";
    str_to_lower(str1);
    assert(strcmp(str1, "hello") == 0);
    
    char str2[] = "MiXeD";
    str_to_lower(str2);
    assert(strcmp(str2, "mixed") == 0);
    
    char str3[] = "already";
    str_to_lower(str3);
    assert(strcmp(str3, "already") == 0);
    
    printf("小写转换测试通过！\n");
}

void test_document_processing() {
    printf("测试文档处理...\n");
    
    // 创建临时测试文件
    FILE *test_file = fopen("test_doc.txt", "w");
    assert(test_file != NULL);
    fprintf(test_file, "The quick brown fox jumps over the lazy dog. ");
    fprintf(test_file, "The dog was really lazy!");
    fclose(test_file);
    
    // 处理文档
    StopWords *sw = stop_words_create();
    Document *doc = document_create("test_doc.txt");
    
    assert(document_load_from_file(doc, "test_doc.txt"));
    assert(doc->content != NULL);
    
    assert(document_process(doc, sw));
    assert(doc->word_count > 0);
    assert(doc->word_freq->unique_words > 0);
    
    // 验证停用词被过滤
    assert(hash_table_get(doc->word_freq, "the") == -1);
    
    // 验证有效词被计数
    assert(hash_table_get(doc->word_freq, "dog") == 2);
    
    // 清理
    document_destroy(doc);
    stop_words_destroy(sw);
    remove("test_doc.txt");
    
    printf("文档处理测试通过！\n");
}

void test_stop_words_file_loading() {
    printf("测试从文件加载停用词...\n");
    
    // 创建临时停用词文件
    FILE *stop_file = fopen("test_stopwords.txt", "w");
    assert(stop_file != NULL);
    fprintf(stop_file, "test1\n");
    fprintf(stop_file, "test2\n");
    fprintf(stop_file, "test3\n");
    fclose(stop_file);
    
    StopWords *sw = stop_words_create();
    size_t initial_size = sw->size;
    
    assert(stop_words_load_from_file(sw, "test_stopwords.txt"));
    assert(sw->size == initial_size + 3);
    
    assert(is_stop_word(sw, "test1") == true);
    assert(is_stop_word(sw, "test2") == true);
    assert(is_stop_word(sw, "test3") == true);
    
    stop_words_destroy(sw);
    remove("test_stopwords.txt");
    
    printf("停用词文件加载测试通过！\n");
}

void test_word_char_detection() {
    printf("测试单词字符检测...\n");
    
    assert(is_word_char('a') == true);
    assert(is_word_char('Z') == true);
    assert(is_word_char('\'') == true);
    
    assert(is_word_char(' ') == false);
    assert(is_word_char('.') == false);
    assert(is_word_char('1') == false);
    assert(is_word_char('!') == false);
    
    printf("单词字符检测测试通过！\n");
}

void test_empty_document() {
    printf("测试空文档处理...\n");
    
    // 创建空文件
    FILE *empty_file = fopen("empty.txt", "w");
    assert(empty_file != NULL);
    fclose(empty_file);
    
    Document *doc = document_create("empty.txt");
    StopWords *sw = stop_words_create();
    
    // 空文件应该加载失败或产生空文档
    bool loaded = document_load_from_file(doc, "empty.txt");
    
    if (loaded) {
        document_process(doc, sw);
        assert(doc->word_count == 0);
    }
    
    document_destroy(doc);
    stop_words_destroy(sw);
    remove("empty.txt");
    
    printf("空文档测试通过！\n");
}

int main() {
    printf("========================================\n");
    printf("文本处理器测试套件\n");
    printf("========================================\n\n");
    
    test_stop_words_basic();
    test_document_creation();
    test_word_tokenization();
    test_str_to_lower();
    test_word_char_detection();
    test_document_processing();
    test_stop_words_file_loading();
    test_empty_document();
    
    printf("\n========================================\n");
    printf("所有测试通过！✓\n");
    printf("========================================\n");
    
    return 0;
}
