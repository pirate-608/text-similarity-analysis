#include "ui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#ifdef _WIN32
    #define CLEAR_COMMAND "cls"
#else
    #define CLEAR_COMMAND "clear"
#endif

// 清屏
void clear_screen() {
    system(CLEAR_COMMAND);
}

// 打印横幅
void print_banner() {
    printf("========================================\n");
    printf("    文本相似度分析系统\n");
    printf("    Text Similarity Analysis System\n");
    printf("========================================\n\n");
}

// 打印菜单
void print_menu() {
    printf("主菜单:\n");
    printf("  1. 加载文档目录\n");
    printf("  2. 比较两个文档\n");
    printf("  3. 生成相似度矩阵\n");
    printf("  4. 显示最相似文档对\n");
    printf("  5. 筛选相似度对\n");
    printf("  6. 显示热力图\n");
    printf("  7. 显示统计信息\n");
    printf("  0. 退出\n");
    printf("请选择操作 (0-7): ");
}

// 获取菜单选择
MenuOption get_menu_choice() {
    char input[10];
    fgets(input, sizeof(input), stdin);
    
    int choice = atoi(input);
    if (choice < 0 || choice > 7) {
        return MENU_EXIT;
    }
    
    return (MenuOption)choice;
}

// 处理菜单选择
void process_menu_choice(MenuOption choice, DocumentCollection **col_ptr, 
                        SimilarityMatrix **matrix_ptr, StopWords **stop_words_ptr) {
    switch (choice) {
        case MENU_EXIT:
            printf("感谢使用！再见！\n");
            break;
            
        case MENU_LOAD_DIR:
            if (*col_ptr) {
                collection_destroy(*col_ptr);
                *col_ptr = NULL;
            }
            if (*matrix_ptr) {
                similarity_matrix_destroy(*matrix_ptr);
                *matrix_ptr = NULL;
            }
            
            {
                char dir_path[256];
                printf("请输入文档目录路径: ");
                fgets(dir_path, sizeof(dir_path), stdin);
                dir_path[strcspn(dir_path, "\n")] = '\0';
                
                *col_ptr = load_documents_from_dir(dir_path, *stop_words_ptr);
                if (*col_ptr) {
                    printf("成功加载 %zu 个文档\n", (*col_ptr)->count);
                }
            }
            break;
            
        case MENU_COMPARE_TWO:
            compare_two_documents();
            break;
            
        case MENU_GENERATE_MATRIX:
            if (*col_ptr && (*col_ptr)->count > 0) {
                if (*matrix_ptr) {
                    similarity_matrix_destroy(*matrix_ptr);
                }
                
                *matrix_ptr = similarity_matrix_create(*col_ptr);
                if (*matrix_ptr) {
                    similarity_matrix_print(*matrix_ptr);
                    
                    char filename[256];
                    printf("请输入CSV文件名 (或按Enter跳过): ");
                    fgets(filename, sizeof(filename), stdin);
                    filename[strcspn(filename, "\n")] = '\0';
                    
                    if (strlen(filename) > 0) {
                        similarity_matrix_save_csv(*matrix_ptr, filename);
                    }
                }
            } else {
                printf("请先加载文档目录！\n");
            }
            break;
            
        case MENU_SHOW_TOP_PAIRS:
            if (*matrix_ptr) {
                char input[10];
                printf("显示前N个最相似对 (默认10): ");
                fgets(input, sizeof(input), stdin);
                
                size_t top_n = atoi(input);
                if (top_n == 0) top_n = 10;
                
                show_top_similarity_pairs(*matrix_ptr, top_n);
            } else {
                printf("请先生成相似度矩阵！\n");
            }
            break;
            
        case MENU_FILTER_PAIRS:
            if (*matrix_ptr) {
                char input[10];
                printf("请输入相似度阈值 (0.0-1.0): ");
                fgets(input, sizeof(input), stdin);
                
                double threshold = atof(input);
                filter_similarity_pairs(*matrix_ptr, threshold);
            } else {
                printf("请先生成相似度矩阵！\n");
            }
            break;
            
        case MENU_SHOW_HEATMAP:
            if (*matrix_ptr) {
                show_heatmap(*matrix_ptr);
            } else {
                printf("请先生成相似度矩阵！\n");
            }
            break;
            
        case MENU_STATISTICS:
            if (*col_ptr) {
                show_statistics(*col_ptr);
            } else {
                printf("请先加载文档目录！\n");
            }
            break;
            
        default:
            printf("无效的选择！\n");
            break;
    }
    
    if (choice != MENU_EXIT) {
        printf("\n按Enter键继续...");
        getchar();
    }
}

// 比较两个文档
void compare_two_documents() {
    char file1[256], file2[256];
    
    printf("请输入第一个文档路径: ");
    fgets(file1, sizeof(file1), stdin);
    file1[strcspn(file1, "\n")] = '\0';
    
    printf("请输入第二个文档路径: ");
    fgets(file2, sizeof(file2), stdin);
    file2[strcspn(file2, "\n")] = '\0';
    
    // 创建停用词表
    StopWords *stop_words = stop_words_create();
    
    // 加载并处理文档
    Document *doc1 = document_create("文档1");
    Document *doc2 = document_create("文档2");
    
    if (document_load_from_file(doc1, file1) &&
        document_load_from_file(doc2, file2)) {
        
        document_process(doc1, stop_words);
        document_process(doc2, stop_words);
        
        // 计算相似度
        double cosine_sim = document_cosine_similarity(doc1, doc2);
        double jaccard_sim = jaccard_similarity(doc1->word_freq, doc2->word_freq);
        
        printf("\n比较结果:\n");
        printf("  余弦相似度: %.4f\n", cosine_sim);
        printf("  Jaccard相似度: %.4f\n", jaccard_sim);
        printf("  文档1单词数: %zu\n", doc1->word_count);
        printf("  文档2单词数: %zu\n", doc2->word_count);
        printf("  文档1唯一单词数: %zu\n", doc1->word_freq->unique_words);
        printf("  文档2唯一单词数: %zu\n", doc2->word_freq->unique_words);
        
        // 显示相似度解释
        printf("\n相似度解释:\n");
        if (cosine_sim >= 0.9) {
            printf("  非常高相似度 (>0.9)\n");
        } else if (cosine_sim >= 0.7) {
            printf("  高相似度 (0.7-0.9)\n");
        } else if (cosine_sim >= 0.5) {
            printf("  中等相似度 (0.5-0.7)\n");
        } else if (cosine_sim >= 0.3) {
            printf("  低相似度 (0.3-0.5)\n");
        } else {
            printf("  非常低相似度 (<0.3)\n");
        }
        
    } else {
        printf("无法加载文档！\n");
    }
    
    // 清理
    stop_words_destroy(stop_words);
    document_destroy(doc1);
    document_destroy(doc2);
}

// 显示统计信息
void show_statistics(DocumentCollection *col) {
    if (!col || col->count == 0) {
        printf("没有文档可显示统计信息！\n");
        return;
    }
    
    printf("文档集合统计信息:\n");
    printf("  文档总数: %zu\n", col->count);
    
    size_t total_words = 0;
    size_t total_unique_words = 0;
    size_t max_words = 0;
    size_t min_words = SIZE_MAX;
    char *longest_doc = NULL;
    char *shortest_doc = NULL;
    
    for (size_t i = 0; i < col->count; i++) {
        Document *doc = col->documents[i];
        total_words += doc->word_count;
        total_unique_words += doc->word_freq->unique_words;
        
        if (doc->word_count > max_words) {
            max_words = doc->word_count;
            longest_doc = doc->filename;
        }
        
        if (doc->word_count < min_words) {
            min_words = doc->word_count;
            shortest_doc = doc->filename;
        }
    }
    
    printf("  总单词数: %zu\n", total_words);
    printf("  平均单词数: %.1f\n", (double)total_words / col->count);
    printf("  总唯一单词数: %zu\n", total_unique_words);
    printf("  平均唯一单词数: %.1f\n", (double)total_unique_words / col->count);
    printf("  最长文档: %s (%zu 单词)\n", longest_doc, max_words);
    printf("  最短文档: %s (%zu 单词)\n", shortest_doc, min_words);
    
    // 打印每个文档的统计信息
    printf("\n各文档详细统计:\n");
    for (size_t i = 0; i < col->count; i++) {
        printf("  %-20s: %zu 单词, %zu 唯一词\n", 
               col->documents[i]->filename,
               col->documents[i]->word_count,
               col->documents[i]->word_freq->unique_words);
    }
}

// 显示前N个最相似对
void show_top_similarity_pairs(SimilarityMatrix *matrix, size_t top_n) {
    size_t result_count;
    SimilarityPair *pairs = find_top_similarities(matrix, top_n, &result_count);
    
    if (!pairs || result_count == 0) {
        printf("没有找到相似度对！\n");
        return;
    }
    
    printf("\n前%zu个最相似文档对:\n", result_count);
    printf("排名 | 文档1                | 文档2                | 相似度\n");
    printf("-----+----------------------+----------------------+--------\n");
    
    for (size_t i = 0; i < result_count; i++) {
        printf("%4zu | %-20s | %-20s | %.4f\n", 
               i + 1, 
               pairs[i].doc1, 
               pairs[i].doc2, 
               pairs[i].similarity);
    }
    
    free(pairs);
}

// 筛选相似度对
void filter_similarity_pairs(SimilarityMatrix *matrix, double threshold) {
    size_t result_count;
    SimilarityPair *pairs = find_top_similarities(matrix, matrix->size * matrix->size, &result_count);
    
    if (!pairs || result_count == 0) {
        printf("没有找到相似度对！\n");
        return;
    }
    
    printf("\n相似度大于 %.2f 的文档对:\n", threshold);
    printf("文档1                | 文档2                | 相似度\n");
    printf("----------------------+----------------------+--------\n");
    
    size_t count = 0;
    for (size_t i = 0; i < result_count; i++) {
        if (pairs[i].similarity >= threshold) {
            printf("%-20s | %-20s | %.4f\n", 
                   pairs[i].doc1, 
                   pairs[i].doc2, 
                   pairs[i].similarity);
            count++;
        }
    }
    
    printf("共找到 %zu 对文档\n", count);
    free(pairs);
}

// 显示ASCII热力图
void show_heatmap(SimilarityMatrix *matrix) {
    if (!matrix) return;
    
    printf("\n相似度热力图:\n");
    
    // 限制显示大小
    size_t display_size = matrix->size > 20 ? 20 : matrix->size;
    
    // 打印列标题
    printf("     ");
    for (size_t j = 0; j < display_size; j++) {
        printf("%2zu ", j + 1);
    }
    printf("\n");
    
    // 打印分隔线
    printf("    +");
    for (size_t j = 0; j < display_size; j++) {
        printf("---");
    }
    printf("\n");
    
    // 打印热力图
    const char *heat_chars = " .:-=+*#%@";
    
    for (size_t i = 0; i < display_size; i++) {
        printf("%2zu | ", i + 1);
        
        for (size_t j = 0; j < display_size; j++) {
            double similarity = matrix->matrix[i][j];
            int index = (int)(similarity * (strlen(heat_chars) - 1));
            
            if (index < 0) index = 0;
            if (index >= (int)strlen(heat_chars)) index = strlen(heat_chars) - 1;
            
            printf("%c  ", heat_chars[index]);
        }
        printf("\n");
    }
    
    // 打印图例
    printf("\n图例:\n");
    for (size_t i = 0; i < strlen(heat_chars); i++) {
        double range_start = (double)i / (strlen(heat_chars) - 1);
        double range_end = (double)(i + 1) / (strlen(heat_chars) - 1);
        printf("  %c : %.2f-%.2f\n", heat_chars[i], range_start, range_end);
    }
    
    if (matrix->size > display_size) {
        printf("\n注意: 只显示了前%zu个文档的热力图\n", display_size);
    }
}