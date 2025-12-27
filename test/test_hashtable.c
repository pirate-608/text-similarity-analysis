#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "hashtable.h"

void test_hash_table_basic() {
    printf("测试基本哈希表操作...\n");
    
    HashTable *table = hash_table_create(10);
    assert(table != NULL);
    assert(table->capacity == 10);
    
    // 测试插入
    assert(hash_table_insert(table, "apple", 5));
    assert(hash_table_insert(table, "banana", 3));
    assert(hash_table_insert(table, "cherry", 7));
    
    // 测试查找
    assert(hash_table_get(table, "apple") == 5);
    assert(hash_table_get(table, "banana") == 3);
    assert(hash_table_get(table, "cherry") == 7);
    assert(hash_table_get(table, "nonexistent") == -1);
    
    // 测试更新
    assert(hash_table_insert(table, "apple", 2)); // 应该累加
    assert(hash_table_get(table, "apple") == 7); // 5 + 2 = 7
    
    // 测试删除
    assert(hash_table_remove(table, "banana"));
    assert(hash_table_get(table, "banana") == -1);
    
    hash_table_destroy(table);
    printf("基本测试通过！\n");
}

void test_hash_table_resize() {
    printf("测试哈希表扩容...\n");
    
    HashTable *table = hash_table_create(5);
    assert(table != NULL);
    
    // 插入多个元素触发扩容
    for (int i = 0; i < 100; i++) {
        char key[20];
        sprintf(key, "key%d", i);
        assert(hash_table_insert(table, key, i));
    }
    
    // 验证所有元素都存在
    for (int i = 0; i < 100; i++) {
        char key[20];
        sprintf(key, "key%d", i);
        assert(hash_table_get(table, key) == i);
    }
    
    hash_table_print_stats(table);
    hash_table_destroy(table);
    printf("扩容测试通过！\n");
}

void test_hash_function() {
    printf("测试哈希函数...\n");
    
    // 测试不同字符串的哈希值
    size_t hash1 = hash_function("hello", 100);
    size_t hash2 = hash_function("world", 100);
    size_t hash3 = hash_function("hello", 100);
    
    assert(hash1 != hash2); // 不同字符串应该有不同的哈希值
    assert(hash1 == hash3); // 相同字符串应该有相同的哈希值
    
    // 测试哈希分布
    int buckets[10] = {0};
    for (int i = 0; i < 1000; i++) {
        char key[20];
        sprintf(key, "key%d", i);
        size_t hash = hash_function(key, 10);
        assert(hash < 10);
        buckets[hash]++;
    }
    
    // 检查分布是否相对均匀
    printf("哈希分布: ");
    for (int i = 0; i < 10; i++) {
        printf("%d ", buckets[i]);
    }
    printf("\n");
    
    printf("哈希函数测试通过！\n");
}

int main() {
    printf("开始哈希表测试...\n\n");
    
    test_hash_function();
    printf("\n");
    
    test_hash_table_basic();
    printf("\n");
    
    test_hash_table_resize();
    printf("\n");
    
    printf("所有测试通过！\n");
    return 0;
}