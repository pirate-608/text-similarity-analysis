#ifndef TEXT_PROCESSOR_H
#define TEXT_PROCESSOR_H

#include "hashtable.h"
#include <stdbool.h>

// 停用词表
typedef struct StopWords {
    char **words;
    size_t size;
    size_t capacity;
} StopWords;

// 文档结构
typedef struct Document {
    char filename[256];
    HashTable *word_freq;
    char *content;
    size_t word_count;
} Document;

// 文本处理函数
Document* document_create(const char *filename);
void document_destroy(Document *doc);
bool document_load_from_file(Document *doc, const char *filename);
bool document_process(Document *doc, StopWords *stop_words);
void document_print_stats(Document *doc);

// 停用词表函数
StopWords* stop_words_create();
bool stop_words_load_from_file(StopWords *sw, const char *filename);
bool stop_words_add(StopWords *sw, const char *word);
bool is_stop_word(StopWords *sw, const char *word);
void stop_words_destroy(StopWords *sw);

// 工具函数
char* str_to_lower(char *str);
bool is_word_char(char c);
char* get_next_word(char **text_ptr);

#endif