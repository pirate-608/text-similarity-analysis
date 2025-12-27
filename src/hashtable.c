#include "hashtable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define LOAD_FACTOR_THRESHOLD 0.75
#define INITIAL_CAPACITY 101

// 创建哈希表
HashTable* hash_table_create(size_t capacity) {
    HashTable *table = (HashTable*)malloc(sizeof(HashTable));
    if (!table) return NULL;
    
    table->capacity = capacity > 0 ? capacity : INITIAL_CAPACITY;
    table->size = 0;
    table->unique_words = 0;
    table->collisions = 0;
    
    table->buckets = (Entry**)calloc(table->capacity, sizeof(Entry*));
    if (!table->buckets) {
        free(table);
        return NULL;
    }
    
    return table;
}

// 销毁哈希表
void hash_table_destroy(HashTable *table) {
    if (!table) return;
    
    for (size_t i = 0; i < table->capacity; i++) {
        Entry *entry = table->buckets[i];
        while (entry) {
            Entry *next = entry->next;
            free(entry->key);
            free(entry);
            entry = next;
        }
    }
    
    free(table->buckets);
    free(table);
}

// 哈希函数 (djb2算法)
size_t hash_function(const char *key, size_t capacity) {
    unsigned long hash = 5381;
    int c;
    
    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; // hash * 33 + c
    }
    
    return hash % capacity;
}

// 插入键值对
bool hash_table_insert(HashTable *table, const char *key, int value) {
    if (!table || !key) return false;
    
    // 检查是否需要扩容
    if (hash_table_load_factor(table) > LOAD_FACTOR_THRESHOLD) {
        hash_table_resize(table);
    }
    
    size_t index = hash_function(key, table->capacity);
    Entry *entry = table->buckets[index];
    
    // 检查是否已存在
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            entry->value += value; // 累加词频
            return true;
        }
        entry = entry->next;
    }
    
    // 创建新节点
    Entry *new_entry = (Entry*)malloc(sizeof(Entry));
    if (!new_entry) {
        fprintf(stderr, "错误: 无法分配内存用于新哈希表条目\n");
        return false;
    }
    
    new_entry->key = strdup(key);
    if (!new_entry->key) {
        fprintf(stderr, "错误: 无法分配内存用于键\n");
        free(new_entry);
        return false;
    }
    
    new_entry->value = value;
    
    // 插入到链表头部
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    
    // 统计碰撞
    if (new_entry->next) {
        table->collisions++;
    }
    
    table->size++;
    table->unique_words++;
    
    return true;
}

// 查找键值
int hash_table_get(HashTable *table, const char *key) {
    if (!table || !key) return -1;
    
    size_t index = hash_function(key, table->capacity);
    Entry *entry = table->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            return entry->value;
        }
        entry = entry->next;
    }
    
    return -1; // 未找到
}

// 删除键值对
bool hash_table_remove(HashTable *table, const char *key) {
    if (!table || !key) return false;
    
    size_t index = hash_function(key, table->capacity);
    Entry *prev = NULL;
    Entry *entry = table->buckets[index];
    
    while (entry) {
        if (strcmp(entry->key, key) == 0) {
            if (prev) {
                prev->next = entry->next;
            } else {
                table->buckets[index] = entry->next;
            }
            
            free(entry->key);
            free(entry);
            table->size--;
            return true;
        }
        prev = entry;
        entry = entry->next;
    }
    
    return false;
}

// 调整哈希表大小
void hash_table_resize(HashTable *table) {
    if (!table) return;
    
    // 防止整数溢出
    if (table->capacity > (SIZE_MAX - 1) / 2) {
        fprintf(stderr, "警告: 哈希表容量已达到最大值，无法扩容\n");
        return;
    }
    
    size_t new_capacity = table->capacity * 2 + 1;
    Entry **new_buckets = (Entry**)calloc(new_capacity, sizeof(Entry*));
    
    if (!new_buckets) {
        fprintf(stderr, "错误: 无法分配内存用于扩容哈希表\n");
        return;
    }
    
    // 重新哈希所有元素
    for (size_t i = 0; i < table->capacity; i++) {
        Entry *entry = table->buckets[i];
        while (entry) {
            Entry *next = entry->next;
            size_t new_index = hash_function(entry->key, new_capacity);
            
            // 插入到新桶
            entry->next = new_buckets[new_index];
            new_buckets[new_index] = entry;
            
            entry = next;
        }
    }
    
    free(table->buckets);
    table->buckets = new_buckets;
    table->capacity = new_capacity;
    table->collisions = 0; // 重置碰撞计数
}

// 计算负载因子
double hash_table_load_factor(HashTable *table) {
    return (double)table->size / table->capacity;
}

// 打印统计信息
void hash_table_print_stats(HashTable *table) {
    if (!table) {
        fprintf(stderr, "错误: 空哈希表指针\n");
        return;
    }
    
    printf("哈希表统计信息:\n");
    printf("  容量: %zu\n", table->capacity);
    printf("  元素数量: %zu\n", table->size);
    printf("  唯一单词数: %zu\n", table->unique_words);
    printf("  碰撞次数: %zu\n", table->collisions);
    printf("  负载因子: %.3f\n", hash_table_load_factor(table));
}