#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#ifdef _WIN32
#include <windows.h>
#endif
#include "hashtable.h"
#include "text_processor.h"
#include "vector_math.h"
#include "file_manager.h"
#include "ui.h"

// 命令行参数处理
typedef struct {
    char *input_dir;
    char *output_file;
    char *stop_words_file;
    int use_gui;
    int batch_mode;
} CommandLineArgs;

// 保证在 Windows 控制台下使用 UTF-8 输出，避免中文乱码
static void setup_console_encoding(void) {
#ifdef _WIN32
    // 先把控制台切换到 UTF-8 代码页
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
#endif

    // 将 C 运行时的多字节编码固定为 UTF-8，避免默认 CP936 造成乱码
    if (!setlocale(LC_ALL, "C.UTF-8")) {
        setlocale(LC_ALL, ".UTF-8");
    }
}

CommandLineArgs parse_arguments(int argc, char *argv[]) {
    CommandLineArgs args = {0};
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-d") == 0 && i + 1 < argc) {
            args.input_dir = argv[++i];
            args.batch_mode = 1;
        } else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            args.output_file = argv[++i];
        } else if (strcmp(argv[i], "-s") == 0 && i + 1 < argc) {
            args.stop_words_file = argv[++i];
        } else if (strcmp(argv[i], "-g") == 0) {
            args.use_gui = 1;
        } else if (strcmp(argv[i], "-h") == 0) {
            printf("用法: %s [选项]\n", argv[0]);
            printf("选项:\n");
            printf("  -d <目录>   指定文档目录路径\n");
            printf("  -o <文件>   指定输出CSV文件\n");
            printf("  -s <文件>   指定停用词文件\n");
            printf("  -g          使用图形界面模式\n");
            printf("  -h          显示此帮助信息\n");
            exit(0);
        }
    }
    
    return args;
}

// 批处理模式
void batch_mode(const char *input_dir, const char *output_file, 
                const char *stop_words_file) {
    printf("批处理模式启动...\n");
    
    // 创建停用词表
    StopWords *stop_words = stop_words_create();
    if (stop_words_file) {
        stop_words_load_from_file(stop_words, stop_words_file);
        printf("已加载停用词文件: %s\n", stop_words_file);
    }
    
    // 加载文档
    DocumentCollection *col = load_documents_from_dir(input_dir, stop_words);
    if (!col || col->count == 0) {
        printf("错误: 无法从目录加载文档\n");
        stop_words_destroy(stop_words);
        return;
    }
    
    printf("成功加载 %zu 个文档\n", col->count);
    
    // 生成相似度矩阵
    SimilarityMatrix *matrix = similarity_matrix_create(col);
    if (!matrix) {
        printf("错误: 无法生成相似度矩阵\n");
        collection_destroy(col);
        stop_words_destroy(stop_words);
        return;
    }
    
    // 保存到文件
    if (output_file) {
        similarity_matrix_save_csv(matrix, output_file);
    } else {
        similarity_matrix_save_csv(matrix, "similarity_matrix.csv");
    }
    
    // 显示前10个最相似对
    size_t result_count;
    SimilarityPair *pairs = find_top_similarities(matrix, 10, &result_count);
    
    if (pairs) {
        printf("\n前10个最相似文档对:\n");
        for (size_t i = 0; i < result_count; i++) {
            printf("%2zu. %-20s <-> %-20s : %.4f\n", 
                   i + 1, 
                   pairs[i].doc1, 
                   pairs[i].doc2, 
                   pairs[i].similarity);
        }
        free(pairs);
    }
    
    // 清理
    similarity_matrix_destroy(matrix);
    collection_destroy(col);
    stop_words_destroy(stop_words);
    
    printf("批处理完成！\n");
}

// 交互模式
void interactive_mode() {
    DocumentCollection *col = NULL;
    SimilarityMatrix *matrix = NULL;
    StopWords *stop_words = stop_words_create();
    
    clear_screen();
    print_banner();
    
    MenuOption choice;
    do {
        clear_screen();
        print_banner();
        print_menu();
        
        choice = get_menu_choice();
        process_menu_choice(choice, &col, &matrix, &stop_words);
        
    } while (choice != MENU_EXIT);
    
    // 清理
    if (col) collection_destroy(col);
    if (matrix) similarity_matrix_destroy(matrix);
    if (stop_words) stop_words_destroy(stop_words);
}

// 主函数
int main(int argc, char *argv[]) {
    setup_console_encoding();

    printf("文本相似度分析系统 v1.0\n");
    printf("=======================\n\n");
    
    // 解析命令行参数
    CommandLineArgs args = parse_arguments(argc, argv);
    
    if (args.batch_mode) {
        // 批处理模式
        if (!args.input_dir) {
            printf("错误: 批处理模式需要指定输入目录 (-d)\n");
            printf("使用 -h 查看帮助信息\n");
            return 1;
        }
        
        batch_mode(args.input_dir, args.output_file, args.stop_words_file);
    } else {
        // 交互模式
        interactive_mode();
    }
    
    return 0;
}