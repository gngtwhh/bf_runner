/**
 * @file: command.c
 * @description: This is a source file that contains the implementation of xxx
 * @author: WAHAHA
 * @Date: 2024/8/7 下午12:58
 * @FilePath: bf_runner//command.c
 * @category: 
 */

#include <stdlib.h>
#include "command.h"
#include "debug_sig.h"

files_t parse_params(int argc, char *argv[]) {
    files_t files = {.code_file=NULL, .input_file=NULL, .output_file=NULL};
    files.code_file = fopen(argv[1], "r");
    if (files.code_file == NULL) {
#if DEBUG_SIG >= DEBUG_SIG_ERROR
        printf("[ERROR] BF code file not found\n");
#endif
        exit(1);
    }
    if (argc >= 3) {
        files.input_file = fopen(argv[2], "r");
        if (files.input_file == NULL) {
#if DEBUG_SIG >= DEBUG_SIG_ERROR
            printf("[ERROR] Input file not found\n");
#endif
            exit(1);
        }
    }
    if (argc >= 4) {
        files.output_file = fopen(argv[3], "w");
        if (files.output_file == NULL) {
#if DEBUG_SIG >= DEBUG_SIG_ERROR
            printf("[ERROR] Output file not found\n");
#endif
            exit(1);
        }
    }
    return files;
}