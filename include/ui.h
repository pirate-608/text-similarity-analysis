#ifndef UI_H
#define UI_H

#include "file_manager.h"

// 菜单选项
typedef enum {
    MENU_EXIT = 0,
    MENU_LOAD_DIR,
    MENU_COMPARE_TWO,
    MENU_GENERATE_MATRIX,
    MENU_SHOW_TOP_PAIRS,
    MENU_FILTER_PAIRS,
    MENU_SHOW_HEATMAP,
    MENU_STATISTICS
} MenuOption;

// 用户界面函数
void clear_screen();
void print_banner();
void print_menu();
MenuOption get_menu_choice();
void process_menu_choice(MenuOption choice, DocumentCollection **col_ptr, 
                        SimilarityMatrix **matrix_ptr, StopWords **stop_words_ptr);
void compare_two_documents();
void show_statistics(DocumentCollection *col);
void show_top_similarity_pairs(SimilarityMatrix *matrix, size_t top_n);
void filter_similarity_pairs(SimilarityMatrix *matrix, double threshold);
void show_heatmap(SimilarityMatrix *matrix);

#endif