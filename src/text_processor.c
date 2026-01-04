#include "text_processor.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// 常见停用词
static const char *default_stop_words[] = {
    "the", "a", "an", "and", "or", "but", "in", "on", "at", "to",
    "for", "of", "with", "by", "is", "are", "was", "were", "be",
    "been", "being", "have", "has", "had", "do", "does", "did",
    "will", "would", "shall", "should", "may", "might", "must",
    "can", "could", "i", "you", "he", "she", "it", "we", "they",
    "me", "him", "her", "us", "them", "my", "your", "his", "its",
    "our", "their", "mine", "yours", "hers", "ours", "theirs",
    "this", "that", "these", "those", "am", "if", "then", "else",
    "when", "where", "why", "how", "all", "any", "both", "each",
    "few", "more", "most", "other", "some", "such", "no", "nor",
    "not", "only", "own", "same", "so", "than", "too", "very"
};

// 创建文档
Document* document_create(const char *filename) {
    Document *doc = (Document*)malloc(sizeof(Document));
    if (!doc) return NULL;
    
    if (filename) {
        strncpy(doc->filename, filename, sizeof(doc->filename) - 1);
        doc->filename[sizeof(doc->filename) - 1] = '\0';
    } else {
        doc->filename[0] = '\0';
    }
    
    doc->word_freq = hash_table_create(101);
    doc->content = NULL;
    doc->word_count = 0;
    
    return doc;
}

// 销毁文档
void document_destroy(Document *doc) {
    if (!doc) return;
    
    if (doc->word_freq) {
        hash_table_destroy(doc->word_freq);
    }
    
    if (doc->content) {
        free(doc->content);
    }
    
    free(doc);
}

// 从文件加载文档
bool document_load_from_file(Document *doc, const char *filename) {
    if (!doc || !filename) return false;
    
    FILE *file = fopen(filename, "rb");
    if (!file) {
        fprintf(stderr, "错误: 无法打开文件 %s\n", filename);
        return false;
    }
    
    // 获取文件大小
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    // 检查文件大小是否合理
    if (file_size < 0) {
        fprintf(stderr, "错误: 无法获取文件大小\n");
        fclose(file);
        return false;
    }
    
    if (file_size == 0) {
        fprintf(stderr, "警告: 文件为空: %s\n", filename);
        fclose(file);
        return false;
    }
    
    // 限制最大文件大小（100MB）
    const long MAX_FILE_SIZE = 100L * 1024 * 1024;
    if (file_size > MAX_FILE_SIZE) {
        fprintf(stderr, "错误: 文件太大 (%ld bytes)，超过限制 (%ld bytes)\n", 
                file_size, MAX_FILE_SIZE);
        fclose(file);
        return false;
    }
    
    // 分配内存
    char *content = (char*)malloc(file_size + 1);
    if (!content) {
        fprintf(stderr, "错误: 无法分配内存用于文件内容\n");
        fclose(file);
        return false;
    }
    
    // 读取文件内容
    size_t bytes_read = fread(content, 1, file_size, file);
    content[bytes_read] = '\0';
    fclose(file);
    
    if (bytes_read != (size_t)file_size) {
        fprintf(stderr, "警告: 读取的字节数与文件大小不匹配\n");
    }
    
    if (doc->content) {
        free(doc->content);
    }
    
    doc->content = content;
    strncpy(doc->filename, filename, sizeof(doc->filename) - 1);
    
    return true;
}

// 处理文档内容
bool document_process(Document *doc, StopWords *stop_words) {
    if (!doc || !doc->content) return false;
    
    char *text = doc->content;
    char *word;
    
    doc->word_count = 0;
    
    while ((word = get_next_word(&text)) != NULL) {
        // 转换为小写
        str_to_lower(word);
        
        // 检查是否是停用词
        if (stop_words && is_stop_word(stop_words, word)) {
            free(word);
            continue;
        }
        
        // 插入到哈希表
        hash_table_insert(doc->word_freq, word, 1);
        doc->word_count++;
        
        free(word);
    }
    
    return true;
}

// 打印文档统计信息
void document_print_stats(Document *doc) {
    printf("文档统计信息: %s\n", doc->filename);
    printf("  总单词数: %zu\n", doc->word_count);
    printf("  唯一单词数: %zu\n", doc->word_freq->unique_words);
    printf("  内容大小: %zu bytes\n", doc->content ? strlen(doc->content) : 0);
}

// 创建停用词表
StopWords* stop_words_create() {
    StopWords *sw = (StopWords*)malloc(sizeof(StopWords));
    if (!sw) return NULL;
    
    sw->capacity = 100;
    sw->size = 0;
    sw->words = (char**)malloc(sw->capacity * sizeof(char*));
    
    if (!sw->words) {
        free(sw);
        return NULL;
    }
    
    // 添加默认停用词
    size_t default_count = sizeof(default_stop_words) / sizeof(default_stop_words[0]);
    for (size_t i = 0; i < default_count; i++) {
        stop_words_add(sw, default_stop_words[i]);
    }
    
    return sw;
}

// 从文件加载停用词
bool stop_words_load_from_file(StopWords *sw, const char *filename) {
    FILE *file = fopen(filename, "r");
    if (!file) return false;
    
    char buffer[256];
    while (fgets(buffer, sizeof(buffer), file)) {
        // 移除换行符
        buffer[strcspn(buffer, "\r\n")] = '\0';
        
        // 转换为小写
        str_to_lower(buffer);
        
        stop_words_add(sw, buffer);
    }
    
    fclose(file);
    return true;
}

// 添加停用词
bool stop_words_add(StopWords *sw, const char *word) {
    if (!sw || !word) return false;
    
    // 检查是否需要扩容
    if (sw->size >= sw->capacity) {
        // 防止整数溢出
        if (sw->capacity > SIZE_MAX / 2) {
            fprintf(stderr, "错误: 停用词表容量已达到最大值\n");
            return false;
        }
        
        size_t new_capacity = sw->capacity * 2;
        char **new_words = realloc(sw->words, new_capacity * sizeof(char*));
        if (!new_words) {
            fprintf(stderr, "错误: 无法扩容停用词表\n");
            return false;
        }
        sw->words = new_words;
        sw->capacity = new_capacity;
    }
    
    sw->words[sw->size] = strdup(word);
    if (!sw->words[sw->size]) {
        fprintf(stderr, "错误: 无法复制停用词\n");
        return false;
    }
    
    sw->size++;
    return true;
}

// 检查是否是停用词
bool is_stop_word(StopWords *sw, const char *word) {
    if (!sw || !word) return false;
    
    for (size_t i = 0; i < sw->size; i++) {
        if (strcmp(sw->words[i], word) == 0) {
            return true;
        }
    }
    
    return false;
}

// 销毁停用词表
void stop_words_destroy(StopWords *sw) {
    if (!sw) return;
    
    for (size_t i = 0; i < sw->size; i++) {
        free(sw->words[i]);
    }
    
    free(sw->words);
    free(sw);
}

// 转换为小写
char* str_to_lower(char *str) {
    if (!str) return str;
    
    for (char *p = str; *p; p++) {
        *p = tolower(*p);
    }
    
    return str;
}

// 检查字符是否是单词字符
bool is_word_char(char c) {
    // 允许英文字母、撇号，以及非ASCII字符（通常是UTF-8多字节字符的一部分）
    // 注意：这只是一个简单的启发式方法，对于中文分词并不完美，
    // 但至少可以保留中文字符而不是将其全部丢弃。
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '\'' || (unsigned char)c >= 0x80;
}

// 获取下一个单词
char* get_next_word(char **text_ptr) {
    if (!text_ptr || !*text_ptr) return NULL;
    
    char *start = *text_ptr;
    
    // 跳过非单词字符
    while (*start && !is_word_char(*start)) {
        start++;
    }
    
    if (!*start) {
        *text_ptr = start;
        return NULL;
    }
    
    char *end = start;
    while (*end && is_word_char(*end)) {
        end++;
    }
    
    // 分配内存存储单词
    size_t length = end - start;
    char *word = (char*)malloc(length + 1);
    if (!word) return NULL;
    
    strncpy(word, start, length);
    word[length] = '\0';
    
    *text_ptr = end;
    return word;
}