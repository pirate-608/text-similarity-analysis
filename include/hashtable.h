#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <stddef.h>
#include <stdbool.h>

// 哈希表节点
typedef struct Entry {
    char *key;
    int value;
    struct Entry *next;
} Entry;

// 哈希表结构
typedef struct HashTable {
    Entry **buckets;
    size_t capacity;
    size_t size;
    size_t unique_words;
    size_t collisions;
} HashTable;

// 哈希表操作函数
HashTable* hash_table_create(size_t capacity);
void hash_table_destroy(HashTable *table);
size_t hash_function(const char *key, size_t capacity);
bool hash_table_insert(HashTable *table, const char *key, int value);
int hash_table_get(HashTable *table, const char *key);
bool hash_table_remove(HashTable *table, const char *key);
void hash_table_resize(HashTable *table);
double hash_table_load_factor(HashTable *table);
void hash_table_print_stats(HashTable *table);

#endif