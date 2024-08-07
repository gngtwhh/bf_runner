/**
 * @file: command.h
 * @description: This is a header file that contains the declaration of xxx
 * @author: WAHAHA
 * @Date: 2024/8/7 下午12:58
 * @FilePath: bf_runner//command.h
 * @category: 
 */

#ifndef BF_RUNNER_COMMAND_H
#define BF_RUNNER_COMMAND_H

#include <stdio.h>

typedef struct files_s {
    FILE *code_file;
    FILE *input_file;
    FILE *output_file;
} files_t;

files_t parse_params(int argc, char *argv[]);

#endif //BF_RUNNER_COMMAND_H
